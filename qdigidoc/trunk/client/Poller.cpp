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
#include <libp11.h>

#include <QApplication>
#include <QStringList>

using namespace digidoc;

Poller::Poller( QObject *parent )
:	QThread( parent )
,	terminate( false )
{
	try { s = new QEstEIDSigner(); }
	catch( const Exception & )
	{
		delete s;
		s = NULL;
	}
}

Poller::~Poller()
{
	stop();
	if( s ) delete s;
}


void Poller::read()
{
	PKCS11_SLOT* slots;
	unsigned int numberOfSlots;

	if( PKCS11_enumerate_slots( (PKCS11_CTX*)s->handle(), &slots, &numberOfSlots ) )
		return;

	cards.clear();
	for( unsigned int i = 0; i < numberOfSlots; ++i )
	{
		PKCS11_SLOT* slot = slots + i;

		if( !slot->token )
			continue;

		QString serialNumber = QByteArray( (const char*)slot->token->serialnr, 16 ).trimmed();
		if( !cards.contains( serialNumber ) )
			cards << serialNumber;
	}
	PKCS11_release_all_slots( (PKCS11_CTX*)s->handle(), slots, numberOfSlots );

	if( !selectedCard.isEmpty() && !cards.contains( selectedCard ) )
	{
		sign = QSslCertificate();
		selectedCard.clear();
	}

	if( selectedCard.isEmpty() && !cards.isEmpty() )
	{
		selectedCard = cards.first();
		readCert();
	}

	Q_EMIT dataChanged( cards, selectedCard, sign );
}

void Poller::readCert()
{
	PKCS11_SLOT* slots;
	unsigned int numberOfSlots;
	if( PKCS11_enumerate_slots( (PKCS11_CTX*)s->handle(), &slots, &numberOfSlots ) != 0 )
		return;

	PKCS11_CERT* certs;
	unsigned int numberOfCerts;
	for( unsigned int i = 0; i < numberOfSlots; ++i )
	{
		PKCS11_SLOT* slot = slots + i;
		if( !slot->token )
			continue;

		if( selectedCard != QByteArray( (const char*)slot->token->serialnr, 16 ).trimmed() )
			continue;

		if( PKCS11_enumerate_certs( slot->token, &certs, &numberOfCerts ) )
			continue;

		if( numberOfCerts <= 0 )
			continue;

		PKCS11_CERT* cert = certs + 0;
		sign = SslCertificate::fromX509( (Qt::HANDLE)cert->x509 );
		break;
	}
	PKCS11_release_all_slots( (PKCS11_CTX*)s->handle(), slots, numberOfSlots );
}

void Poller::run()
{
	s->loadDriver( Conf::getInstance()->getPKCS11DriverPath() );
	read();

	while( !terminate )
	{
		sleep( 1 );

		m.lock();
		if( !select.isEmpty() && cards.contains( select ) )
		{
			selectedCard = select;
			readCert();
			Q_EMIT dataChanged( cards, selectedCard, sign );
		}
		select.clear();
		m.unlock();

		read();
	}
}

void Poller::selectCard( const QString &card )
{
	m.lock();
	select = card;
	m.unlock();
}

void Poller::stop()
{
	terminate = true;
	wait();

	if( s )
		s->unloadDriver();
}

QEstEIDSigner::QEstEIDSigner( const QString &card ) throw(SignException)
:	PKCS11Signer()
,	selectedCard( card )
{}

std::string QEstEIDSigner::getPin( PKCS11Cert c ) throw(SignException)
{
	PinDialog p( PinDialog::Pin2Type, SslCertificate::fromX509( (Qt::HANDLE)c.cert ), qApp->activeWindow() );
	if( !p.exec() )
		throw SignException( __FILE__, __LINE__,
			QObject::tr("PIN acquisition canceled.").toUtf8().constData() );
	return p.text().toStdString();
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
