/*
 * QDigiDocClient
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

#include "QSigner.h"

#include "common/PinDialog.h"
#include "common/SslCertificate.h"

#include <digidocpp/Conf.h>

#include <libp11.h>

#include <QApplication>
#include <QMutex>
#include <QStringList>

#define CKR_OK					(0)
#define CKR_CANCEL				(1)
#define CKR_FUNCTION_CANCELED	(0x50)
#define CKR_PIN_INCORRECT		(0xa0)
#define CKR_PIN_LOCKED			(0xa4)

class QSignerPrivate
{
public:
	QSignerPrivate(): login(false), terminate(false), handle(0), slotCount(0), loginResult(CKR_OK) {}
	volatile bool	login, terminate;
	QMutex			m;
	QHash<QString,unsigned int>	cards;
	QString			selectedCard, select;
	QSslCertificate	sign;
	PKCS11_CTX		*handle;
	PKCS11_SLOT     *slot, *slots;
	unsigned int	slotCount;
	unsigned long	loginResult;
};



using namespace digidoc;

QSigner::QSigner( QObject *parent )
:	QThread( parent )
,	d( new QSignerPrivate )
{}

QSigner::~QSigner() { unloadDriver(); delete d; }

X509* QSigner::getCert() throw(digidoc::SignException) { return (X509*)d->sign.handle(); }

bool QSigner::loadDriver()
{
	std::string name;
	try
	{
		name = Conf::getInstance()->getPKCS11DriverPath();
	}
	catch( const Exception &e )
	{
		Q_EMIT error( tr("Failed to load PKCS#11 module") );
		return false;
	}

	if( !d->handle &&
		(!(d->handle = PKCS11_CTX_new()) || PKCS11_CTX_load( d->handle, name.c_str() )) )
	{
		PKCS11_CTX_free( d->handle );
		d->handle = 0;
		return false;
	}
	if( !isRunning() )
		start();
	return true;
}

void QSigner::read()
{
	if( d->slotCount )
		PKCS11_release_all_slots( d->handle, d->slots, d->slotCount );
	if( PKCS11_enumerate_slots( d->handle, &d->slots, &d->slotCount ) )
		return;

	d->cards.clear();
	for( unsigned int i = 0; i < d->slotCount; ++i )
	{
		PKCS11_SLOT* slot = &d->slots[i];
		if( !slot->token )
			continue;

		QString serialNumber = QByteArray( (const char*)slot->token->serialnr, 16 ).trimmed();
		if( !d->cards.contains( serialNumber ) )
			d->cards[serialNumber] = i + 1;
	}

	if( !d->selectedCard.isEmpty() && !d->cards.contains( d->selectedCard ) )
	{
		d->sign = QSslCertificate();
		d->selectedCard.clear();
	}

	if( d->selectedCard.isEmpty() && !d->cards.isEmpty() )
	{
		d->selectedCard = d->cards.keys().first();
		readCert();
	}

	Q_EMIT dataChanged( d->cards.keys(), d->selectedCard, d->sign );
}

void QSigner::readCert()
{
	Q_EMIT dataChanged( d->cards.keys(), d->selectedCard, d->sign );
	PKCS11_CERT* certs;
	unsigned int numberOfCerts;
	for( unsigned int i = 0; i < d->slotCount; ++i )
	{
		PKCS11_SLOT* slot = &d->slots[i];
		if( !slot->token ||
			d->selectedCard != QByteArray( (const char*)slot->token->serialnr, 16 ).trimmed() ||
			PKCS11_enumerate_certs( slot->token, &certs, &numberOfCerts ) ||
			numberOfCerts <= 0 )
			continue;

		SslCertificate cert = SslCertificate::fromX509( (Qt::HANDLE)(&certs[0])->x509 );
		if( cert.keyUsage().keys().contains( SslCertificate::NonRepudiation ) )
		{
			d->sign = cert;
			d->cards[d->selectedCard] = i;
			d->slot = slot;
			break;
		}
	}
}

void QSigner::run()
{
	d->terminate = false;

	if( !loadDriver() )
	{
		Q_EMIT error( tr("Failed to load PKCS#11 module") );
		return;
	}

	read();
	while( !d->terminate )
	{
		sleep( 1 );

		if( d->m.tryLock() )
		{
			if( !d->select.isEmpty() && d->cards.contains( d->select ) )
			{
				d->selectedCard = d->select;
				readCert();
				Q_EMIT dataChanged( d->cards.keys(), d->selectedCard, d->sign );
			}
			d->select.clear();

			read();
			d->m.unlock();
		}

		if( d->login )
		{
			if( PKCS11_login( d->slot, 0, NULL ) < 0 )
				d->loginResult = ERR_get_error();
			d->login = false;
		}
	}
}

void QSigner::selectCard( const QString &card )
{
	d->m.lock();
	d->select = card;
	d->m.unlock();
}

void QSigner::sign( const Digest &digest, Signature &signature ) throw(digidoc::SignException)
{
	d->m.lock();
	if( d->sign.isNull() || !d->slot || !d->slot->token )
		throwException( tr("Signing certificate is not selected."), 0, Exception::NoException, __LINE__ );

	if( d->slot->token->loginRequired )
	{
		unsigned long err = CKR_OK;
		if( d->slot->token->secureLogin )
		{
			d->login = true;
			PinDialog *p = new PinDialog( PinDialog::Pin2PinpadType, d->sign, qApp->activeWindow() );
			p->show();
			do
			{
				qApp->thread()->wait( 1 );
				qApp->processEvents();
			} while( d->login );
			p->deleteLater();
			err = d->loginResult;
		}
		else
		{
			PinDialog p( PinDialog::Pin2Type, d->sign, qApp->activeWindow() );
			if( !p.exec() )
				throwException( tr("PIN acquisition canceled."), 0, Exception::PINCanceled, __LINE__ );
			if( PKCS11_login( d->slot, 0, p.text().toUtf8() ) < 0 )
				err = ERR_get_error();
		}
		switch( ERR_GET_REASON(err) )
		{
		case CKR_OK: break;
		case CKR_CANCEL:
		case CKR_FUNCTION_CANCELED:
			throwException( tr("PIN acquisition canceled."), 0, Exception::PINCanceled, __LINE__ );
		case CKR_PIN_INCORRECT:
			throwException( tr("PIN Incorrect"), 0, Exception::PINIncorrect, __LINE__ );
		case CKR_PIN_LOCKED:
			throwException( tr("PIN Locked"), 0, Exception::PINLocked, __LINE__ );
		default:
			throwException( tr("Failed to login token"), err, Exception::NoException, __LINE__ );
		}
	}

	PKCS11_CERT *certs;
	unsigned int certCount;
	if( PKCS11_enumerate_certs( d->slot->token, &certs, &certCount ) )
		throwException( tr("Failed to sign document"), ERR_get_error(), Exception::NoException, __LINE__ );
	if( !certCount || !&certs[0] )
		throwException( tr("Failed to sign document") + "\nNo sertificates", 0, Exception::NoException, __LINE__ );

	PKCS11_KEY *key = PKCS11_find_key( &certs[0] );
	if( !key )
		throwException( tr("Failed to sign document") + "\nNo keys", 0, Exception::NoException, __LINE__ );

	if( PKCS11_sign(digest.type, digest.digest, digest.length, signature.signature, &(signature.length), key) != 1 )
		throwException( tr("Failed to sign document"), ERR_get_error(), Exception::NoException, __LINE__ );

	d->m.unlock();
}

void QSigner::throwException( const QString &msg, unsigned long err, Exception::ExceptionCode code, int line ) throw(SignException)
{
	d->m.unlock();
	QString t = msg;
	if( err )
	{
		t += "\n";
		t += QString::fromUtf8( ERR_error_string( err, NULL ) );
	}
	SignException e( __FILE__, line, t.toUtf8().constData() );
	e.setCode( code );
	throw e;
}

void QSigner::unloadDriver()
{
	d->terminate = true;
	wait();
	if( d->slotCount )
		PKCS11_release_all_slots( d->handle, d->slots, d->slotCount );
	d->slotCount = 0;
	if( d->handle )
	{
		PKCS11_CTX_unload( d->handle );
		PKCS11_CTX_free( d->handle );
		d->handle = 0;
	}
}
