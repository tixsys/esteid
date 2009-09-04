/*
 * QEstEidUtil
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

#include "sslConnect.h"

#include "PinDialog.h"
#include "Settings.h"
#include "SslCertificate.h"

#include <libp11.h>
#include <openssl/ssl.h>
#include <stdexcept>
#include <sstream>
#include <QApplication>
#include <QDateTime>

#ifndef PKCS11_MODULE
#  if defined(_WIN32)
#    define PKCS11_MODULE "opensc-pkcs11.dll"
#  else
#    define PKCS11_MODULE "opensc-pkcs11.so"
#  endif
#endif

#ifdef OPENSSL_SYS_WIN32 
#undef X509_NAME 
#undef X509_EXTENSIONS 
#undef X509_CERT_PAIR 
#undef PKCS7_ISSUER_AND_SERIAL 
#undef OCSP_REQUEST 
#undef OCSP_RESPONSE 
#endif

// PKCS#11
#define CKR_OK					(0)
#define CKR_CANCEL				(1)
#define CKR_FUNCTION_CANCELED	(0x50)



class sslError: public std::runtime_error
{
public:
	sslError() : std::runtime_error("openssl error")
	{
		unsigned long err = ERR_get_error();
		std::ostringstream buf;
		buf << ERR_func_error_string( err ) << " (" << ERR_reason_error_string( err ) << ")";
		desc = buf.str();
	}
    ~sslError() throw () {}

	virtual const char *what() const throw() { return desc.c_str(); }
	void static check( bool result ) { if( !result ) throw sslError(); }

	std::string desc;
};



class SSLObj
{
public:
	SSLObj();
	~SSLObj();

	bool connectToHost( const std::string &site );
	QByteArray getUrl( const std::string &url ) const;
	QByteArray getRequest( const std::string &request ) const;

	PKCS11_CTX *ctx;
	SSL		*s;

	std::string card;
	QString pin;
	int		reader;

	unsigned int nslots;
	PKCS11_SLOT *pslots;
};



SSLConnect::SSLConnect( QObject *parent )
:	QObject( parent )
,	obj( new SSLObj() )
{}

SSLConnect::~SSLConnect() { delete obj; }

bool SSLConnect::isLoaded() { return obj && obj->ctx; }

QByteArray SSLConnect::getUrl( RequestType type, const std::string &value )
{
	if ( !isLoaded() )
		return QByteArray();

	std::string site;
	switch( type )
	{
		case AccessCert:
		case MobileInfo: site = SK; break;
		default: site = EESTI; break;
	}
	if ( obj->connectToHost( site ) )
	{
		switch( type )
		{
			case AccessCert: return obj->getRequest( getValue( type ) );
			case EmailInfo: return obj->getUrl( "/idportaal/postisysteem.naita_suunamised" );
			case ActivateEmails: return obj->getUrl( "/idportaal/postisysteem.lisa_suunamine?" + value );
			case MobileInfo: return obj->getRequest( value );
			case PictureInfo: return obj->getUrl( "/idportaal/portaal.idpilt" );
		}
	}
	return QByteArray();
}

QString SSLConnect::pin() const { return obj->pin; }
void SSLConnect::setCard( const std::string &card ) { obj->card = card; }
void SSLConnect::setPin( const QString &pin ) { obj->pin = pin; }
void SSLConnect::setReader( int reader ) { obj->reader = reader; }



SSLObj::SSLObj()
:	ctx( NULL )
,	s( NULL )
,	reader( -1 )
,	nslots( 0 )
{
	ctx = PKCS11_CTX_new();
	if ( PKCS11_CTX_load(ctx, PKCS11_MODULE) )
		throw std::runtime_error( QObject::tr( "failed to load pkcs11 module" ).toStdString() );

	int result = PKCS11_enumerate_slots(ctx, &pslots, &nslots);
	if ( result )
		throw std::runtime_error( QObject::tr( "failed to list slots" ).toStdString() );
}

SSLObj::~SSLObj()
{
	if ( s != NULL )
	{
		SSL_shutdown(s);
		SSL_free(s);
	}
	if ( ctx != NULL )
	{
		if( nslots )
			PKCS11_release_all_slots(ctx, pslots, nslots);
		PKCS11_CTX_unload(ctx);
		PKCS11_CTX_free(ctx);
	}

	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
	ERR_remove_state(0);
}

bool SSLObj::connectToHost( const std::string &site )
{
	PKCS11_SLOT *slot = 0;
	if( reader >= 0 )
	{
		if ( (unsigned int)reader*4 > nslots )
			throw std::runtime_error( QObject::tr( "token failed" ).toStdString() );

		slot = &pslots[reader*4];
		if (!slot || !slot->token)
			throw std::runtime_error( QObject::tr("no token available").toStdString() );
	}
	else
	{
		PKCS11_SLOT *tmp;
		for( unsigned int i = 0; i < nslots; ++i )
		{
			tmp = &pslots[i];
			if( !tmp->token )
				continue;
			if( card.compare( tmp->token->serialnr ) == 0 )
			{
				slot = tmp;
				break;
			}
		}
		if( !slot )
			throw std::runtime_error( QObject::tr("no token available").toStdString() );
	}

	PKCS11_CERT *certs;
	unsigned int ncerts;
	int result = PKCS11_enumerate_certs(slot->token, &certs, &ncerts);
	if( !ncerts )
		throw std::runtime_error( QObject::tr("no certificate available").toStdString() );
	PKCS11_CERT *authcert = &certs[0];

	if( slot->token->loginRequired )
	{
		if( !slot->token->secureLogin )
		{
			if( pin.isNull() )
			{
				PinDialog p( PinDialog::Pin1Type,
					SslCertificate::fromX509( Qt::HANDLE(authcert->x509) ), qApp->activeWindow() );
				if( !p.exec() )
					throw std::runtime_error( "" );
				pin = p.text();
			}
			QCoreApplication::processEvents();
			result = PKCS11_login(slot, 0, pin.toUtf8());
		}
		else
		{
			PinDialog *p = new PinDialog( PinDialog::Pin1PinpadType,
				SslCertificate::fromX509( Qt::HANDLE(authcert->x509) ), qApp->activeWindow() );
			p->show();
			QCoreApplication::processEvents();
			result = PKCS11_login( slot, 0, NULL );
			p->deleteLater();
			pin.clear();
		}
		switch( ERR_GET_REASON( ERR_get_error() ) )
		{
		case CKR_OK: break;
		case CKR_CANCEL:
		case CKR_FUNCTION_CANCELED:
			throw std::runtime_error( "" ); break;
		default:
			throw std::runtime_error( "PIN1Invalid" ); break;
		}
	}

	PKCS11_KEY *authkey = PKCS11_find_key( authcert );
	if ( !authkey )
		throw std::runtime_error( QObject::tr("no key matching certificate available").toStdString() );

	EVP_PKEY *pkey = PKCS11_get_private_key( authkey );

	SSL_CTX *sctx = SSL_CTX_new( SSLv23_client_method() );
	sslError::check( SSL_CTX_use_certificate(sctx, authcert->x509 ) );
	sslError::check( SSL_CTX_use_PrivateKey(sctx,pkey) );
	sslError::check( SSL_CTX_check_private_key(sctx) );
	sslError::check( SSL_CTX_ctrl( sctx, SSL_CTRL_MODE, SSL_MODE_AUTO_RETRY, NULL ) );
	if ( pkey != NULL )
		EVP_PKEY_free(pkey); 

	s = SSL_new(sctx);

	BIO *sock = BIO_new_connect( (char*)site.c_str() );
	BIO_set_conn_port( sock, "https" );
	if( BIO_do_connect( sock ) <= 0 )
		throw std::runtime_error( QObject::tr( "Failed to connect to host. Are you connected to the internet?" ).toStdString() );

	SSL_set_bio( s, sock, sock );
	sslError::check( SSL_connect(s) );
	return true;
}

QByteArray SSLObj::getUrl( const std::string &url ) const
{
	std::string req = "GET " + url + " HTTP/1.0\r\n\r\n";
	return getRequest( req );
}

QByteArray SSLObj::getRequest( const std::string &request ) const
{
	sslError::check( SSL_write( s, request.c_str(), request.length() ) );

	int bytesRead = 0;
	char readBuffer[4096];
	QByteArray buffer;
	do
	{
		bytesRead = SSL_read( s, &readBuffer, 4096 );

		if( bytesRead <= 0 )
		{
			switch( SSL_get_error( s, bytesRead ) )
			{
			case SSL_ERROR_NONE:
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
			case SSL_ERROR_ZERO_RETURN: // Disconnect
				break;
			default:
				sslError::check( 0 );
				break;
			}
		}

		if( bytesRead > 0 )
			buffer += QByteArray( (const char*)&readBuffer, bytesRead );
	} while( bytesRead > 0 );

	int pos = buffer.indexOf( "\r\n\r\n" );
	return pos ? buffer.mid( pos + 4 ) : buffer;
}

std::string SSLConnect::getValue( RequestType type )
{
	std::string value;

	switch( type )
	{
		case AccessCert:
			value += "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
						"<SOAP-ENV:Body>"
						"<m:GetAccessToken xmlns:m=\"urn:GetAccessToken\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
						"<Language xsi:type=\"xsd:string\">ET</Language>"
						"<RequestTime xsi:type=\"xsd:string\" />"
						"<SoftwareName xsi:type=\"xsd:string\">DigiDoc3</SoftwareName>"
						"<SoftwareVersion xsi:type=\"xsd:string\" />"
						"</m:GetAccessToken>"
						"</SOAP-ENV:Body>"
						"</SOAP-ENV:Envelope>";
			value = "POST /id/GetAccessTokenWSProxy/ HTTP/1.1\r\n"
					 "Host: " + QString(SK).toStdString() + "\r\n"
					 "Content-Type: text/xml\r\n"
					 "Content-Length: " + QString::number( value.size() ).toStdString() + "\r\n"
					 "SOAPAction: \"\"\r\n"
					 "Connection: close\r\n\r\n" + value;
		break;
		default: break;
	}
	return value;
}
