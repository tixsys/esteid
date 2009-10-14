/*
 * QDigiDocCrypto
 *
 * Copyright (C) 2009 Jargo Kõster <jargo@innovaatik.ee>
 * Copyright (C) 2009 Raul Metsma <raul@innovaatik.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "Poller.h"

#include "common/SslCertificate.h"

#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocPKCS11.h>

#include <QStringList>

Poller::Poller( QObject *parent )
:	QThread( parent )
,	terminate( false )
{}

Poller::~Poller() { stop(); }

void Poller::read()
{
	CK_ULONG count = 0;
	int err = GetSlotIds( NULL, &count );
	if( err != ERR_OK )
		return;

	CK_SLOT_ID_PTR slotids = (CK_SLOT_ID_PTR)malloc(count*sizeof(CK_SLOT_ID));
	err = GetSlotIds( slotids, &count );
	if( err != ERR_OK )
	{
		free( slotids );
		return;
	}

	cards.clear();
	tokens.clear();
	for( CK_ULONG i = 0; i < count; ++i )
	{
		CK_TOKEN_INFO tokeninfo;
		err = GetTokenInfo( &tokeninfo, slotids[i] );
		QString serialNumber = QByteArray( (const char*)tokeninfo.serialNumber, 16 ).trimmed();
		if( !cards.contains( serialNumber ) )
		{
			cards[serialNumber] = slotids[i];
			tokens[serialNumber] = i;
		}
	}
	free( slotids );

	if( !selectedCard.isEmpty() && !cards.contains( selectedCard ) )
	{
		auth = QSslCertificate();
		selectedCard.clear();
	}

	if( selectedCard.isEmpty() && !cards.isEmpty() )
	{
		selectedCard = cards.begin().key();
		readCert();
	}

	Q_EMIT dataChanged( cards.keys(), selectedCard, auth );
}

void Poller::readCert()
{
	Q_EMIT dataChanged( cards.keys(), selectedCard, auth );
	X509 *cert = NULL;
	GetSlotCertificate( cards[selectedCard], &cert );
	auth = SslCertificate::fromX509( (Qt::HANDLE)cert );
	X509_free( cert );
}

void Poller::run()
{
	terminate = false;
	char driver[200];
	qsnprintf( driver, sizeof(driver), "DIGIDOC_DRIVER_%d_FILE",
		ConfigItem_lookup_int( "DIGIDOC_DEFAULT_DRIVER", 1 ) );
	lib = (Qt::HANDLE)initPKCS11Library( ConfigItem_lookup( driver ) );

	if( !lib )
		return;

	read();

	/*CK_SLOT_ID slot;
	int seq = 0;*/
	while( !terminate )
	{
		sleep( 1 );

		m.lock();
		if( !select.isEmpty() && cards.contains( select ) )
		{
			selectedCard = select;
			readCert();
			Q_EMIT dataChanged( cards.keys(), selectedCard, auth );
		}
		select.clear();
		m.unlock();

		read();
/*		switch( WaitSlotEvent( &slot ) )
		{
		case CKR_OK:
			if( seq == 0 )
				read();
			seq++;
			if( seq == 3 )
				seq = 0;
			break;
		case CKR_NO_EVENT:
			seq = 0;
			break;
		default:
			return;
		}*/
	}
}

void Poller::selectCard( const QString &card )
{
	m.lock();
	select = card;
	m.unlock();
}

quint64 Poller::slot( const QString &card ) const { return cards.value( card ); }
quint64 Poller::token( const QString &card ) const { return tokens.value( card ); }

void Poller::stop()
{
	terminate = true;
	wait();
	if( lib )
		closePKCS11Library( (LIBHANDLE)lib, 0 );
}
