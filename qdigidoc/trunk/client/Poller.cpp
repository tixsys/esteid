/*
 * QDigiDocClient
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

#include "common/PinDialog.h"
#include "common/SslCertificate.h"

#include <digidocpp/XmlConf.h>

#include <QApplication>
#include <QStringList>

using namespace digidoc;

Poller::Poller( QObject *parent )
:	QThread( parent )
,	terminate( false )
{}

Poller::~Poller()
{
	terminate = true;
	wait();
}

void Poller::lock() { m.lock(); }

void Poller::run()
{
	Q_FOREVER
	{
		if( m.tryLock() )
		{
			QEstEIDSigner *s = NULL;
			try { s = new QEstEIDSigner(); }
			catch( const Exception & )
			{
				delete s;
				m.unlock();
				continue;
			}

			if( !selectedCard.isEmpty() && !s->cards().contains( selectedCard ) )
				selectedCard.clear();
			if( selectedCard.isEmpty() && !s->cards().isEmpty() )
				selectedCard = s->cards().first();
			Q_EMIT dataChanged( s->cards(), selectedCard, s->signCert( selectedCard ) );

			delete s;
			m.unlock();
		}

		for( int i = 0; i < 5; ++i )
		{
			if( terminate )
				return;
			sleep( 1 );
		}
	}
}

void Poller::selectCard( const QString &card )
{
	m.lock();
	selectedCard = card;
	m.unlock();
}

void Poller::unlock() { m.unlock(); }


QEstEIDSigner::QEstEIDSigner( const QString &card ) throw(SignException)
:	PKCS11Signer()
,	selectedCard( card )
{
	try	{ getCert(); }
	catch( const Exception & ) {}
}

QStringList	QEstEIDSigner::cards() const
{ return sign.keys(); }
QSslCertificate QEstEIDSigner::signCert( const QString &card ) const { return sign.value(card); }

std::string QEstEIDSigner::getPin( PKCS11Cert c ) throw(SignException)
{
	PinDialog p( PinDialog::Pin2Type, SslCertificate::fromX509( (Qt::HANDLE)c.cert ), qApp->activeWindow() );
	if( !p.exec() )
		throw SignException( __FILE__, __LINE__,
			QObject::tr("PIN acquisition canceled.").toUtf8().constData() );
	return p.textValue().toStdString();
}

PKCS11Signer::PKCS11Cert QEstEIDSigner::selectSigningCertificate(
	std::vector<PKCS11Signer::PKCS11Cert> certificates ) throw(SignException)
{
	PKCS11Signer::PKCS11Cert signCert;
	Q_FOREACH( const PKCS11Signer::PKCS11Cert &cert, certificates )
	{
		try
		{
			const QString card = QString::fromStdString( cert.token.serialNr );
			if( selectedCard.isEmpty() )
				selectedCard = card;
			if( cert.label == std::string("Allkirjastamine") )
			{
				sign[card] = SslCertificate::fromX509( (Qt::HANDLE)cert.cert );
				if( selectedCard == card )
					signCert = cert;
			}
		}
		catch( const Exception & ) {}
	}
	if( signCert.label != std::string("Allkirjastamine") )
		SignException( __FILE__, __LINE__, "Could not find certificate with label 'Allkirjastamine'." );

	return signCert;
}
