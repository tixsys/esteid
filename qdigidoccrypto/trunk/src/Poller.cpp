/*
 * QDigiDocCrypto
 *
 * Copyright (C) 2009 Jargo K�ster <jargo@innovaatik.ee>
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

#include "SslCertificate.h"

#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocPKCS11.h>

#include <QHash>

Poller::Poller( QObject *parent )
:	QThread( parent )
,	terminate( false )
{}

Poller::~Poller()
{
	m.lock();
	terminate = true;
	m.unlock();
	wait();
}

void Poller::lock() { m.lock(); }

void Poller::run()
{
	Q_FOREVER
	{
		sleep( 5 );
		if( !m.tryLock() )
			continue;

		if( terminate )
			return;

		char driver[200];
		snprintf( driver, sizeof(driver), "DIGIDOC_DRIVER_%d_FILE",
			ConfigItem_lookup_int( "DIGIDOC_DEFAULT_DRIVER", 1 ) );
		LIBHANDLE lib = initPKCS11Library( ConfigItem_lookup( driver ) );

		if( !lib )
		{
			m.unlock();
			continue;
		}

		CK_ULONG count = 20;
		CK_SLOT_ID slotids[20];
		int err = GetSlotIds( (CK_SLOT_ID*)&slotids, &count );
		if( err != ERR_OK )
		{
			m.unlock();
			closePKCS11Library( lib, 0 );
			continue;
		}

		QHash<QString,int> cards;
		for( int i = 0; i < count; ++i )
		{
			CK_TOKEN_INFO tokeninfo;
			err = GetTokenInfo( &tokeninfo, slotids[i] );
			QString serialNumber = (char*)tokeninfo.serialNumber;
			if( !cards.contains( serialNumber ) )
				cards[serialNumber] = i;
		}
		closePKCS11Library( lib, 0 );

		if( selectedCard.isEmpty() && !cards.isEmpty() )
		{
			selectedCard = cards.begin().key();
			int slot = cards.begin().value();
			X509 *cert;
			findUsersCertificate( slot, &cert );
			QSslCertificate auth = SslCertificate::fromX509( (Qt::HANDLE*)cert );
			free( cert );
			findUsersCertificate( slot + 1, &cert );
			QSslCertificate sign = SslCertificate::fromX509( (Qt::HANDLE*)cert );
			free( cert );
			Q_EMIT dataChanged( auth, sign );
		}
		else if( !selectedCard.isEmpty() && !cards.contains( selectedCard ) )
		{
			selectedCard.clear();
			Q_EMIT dataChanged( QSslCertificate(), QSslCertificate() );
		}
		m.unlock();
	}
}

void Poller::unlock() { m.unlock(); }
