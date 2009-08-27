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

#include "MobileDialog.h"
#include "DigiDoc.h"

#include <common/Settings.h>
#include <common/SslCertificate.h>

#include <digidocpp/Document.h>
#include <digidocpp/Exception.h>
#include <digidocpp/crypto/Digest.h>
#include <digidocpp/WDoc.h>

#include <QDebug>
#include <QDir>
#include <QSslError>
#include <QSslKey>
#include <QSslSocket>
#include <QTemporaryFile>

MobileDialog::MobileDialog( DigiDoc *doc, QWidget *parent )
:   QDialog( parent )
,   m_doc( doc )
,   m_timer( 0 )
,	statusTimer( 0 )
,   sessionCode( 0 )
{
    setupUi( this );

	Settings s;
	s.beginGroup( "Client" );

	QString certFile = m_doc->getConfValue( DigiDoc::PKCS12Cert, s.value( "pkcs12Cert" ) );
	QString certPass = m_doc->getConfValue( DigiDoc::PKCS12Pass, s.value( "pkcs12Pass" ) );

	m_http = new QHttp( this );

	if ( certFile.size() && QFile::exists( certFile ) )
	{
		QFile f( certFile );
		if( f.open( QIODevice::ReadOnly ) )
		{
			QSslSocket *ssl = new QSslSocket( this );
			QByteArray pkcs12Cert = f.readAll();
			ssl->setPrivateKey( SslCertificate::keyFromPKCS12( pkcs12Cert, certPass.toLatin1() ) );
			ssl->setLocalCertificate( SslCertificate::fromPKCS12( pkcs12Cert, certPass.toLatin1() ) );
			m_http->setSocket( ssl );
		}
	}
    connect( m_http, SIGNAL(requestFinished(int, bool)), SLOT(httpRequestFinished(int, bool)) );
    connect( m_http, SIGNAL(sslErrors(const QList<QSslError> &)), SLOT(sslErrors(const QList<QSslError> &)) );

	m_http->setProxy( s.value( "proxyHost" ).toString(), s.value( "proxyPort" ).toInt() );

	if ( m_doc->documentType() == digidoc::WDoc::BDocType )
		m_http->setHost( "www.openxades.org", QHttp::ConnectionModeHttps, 8443 );
	else
		m_http->setHost( "digidocservice.sk.ee", QHttp::ConnectionModeHttps );

	mobileResults[ "START" ] = tr( "Signing in process" );
	mobileResults[ "REQUEST_OK" ] = tr( "Request accepted" );
	mobileResults[ "EXPIRED_TRANSACTION" ] = tr( "Request timeout" );
	mobileResults[ "USER_CANCEL" ] = tr( "User denied or cancelled" );
	mobileResults[ "SIGNATURE" ] = tr( "Got signature" );
	mobileResults[ "OUTSTANDING_TRANSACTION" ] = tr( "Request pending" );
	mobileResults[ "MID_NOT_READY" ] = tr( "Mobile-ID not ready, try again later" );
	mobileResults[ "PHONE_ABSENT" ] = tr( "Phone absent" );
	mobileResults[ "SENDING_ERROR" ] = tr( "Request sending error" );
	mobileResults[ "SIM_ERROR" ] = tr( "SIM error" );
	mobileResults[ "INTERNAL_ERROR" ] = tr( "Service internal error" );
	mobileResults[ "HOSTNOTFOUND" ] = tr( "Connecting to SK server failed!\nPlease check your internet connection." );
	mobileResults[ "User is not a Mobile-ID client" ] = tr( "User is not a Mobile-ID client" );

	statusTimer = new QTimer( this );
	connect( statusTimer, SIGNAL(timeout()), SLOT(updateStatus()) );
}

void MobileDialog::sslErrors(const QList<QSslError> &)
{ m_http->ignoreSslErrors(); }

void MobileDialog::httpRequestFinished( int id, bool error )
{
    if ( error)
    {
		qDebug() << "Download failed: " << m_http->errorString() << m_http->error();
		if ( m_http->error() == QHttp::HostNotFound )
			labelError->setText( mobileResults.value( "HOSTNOTFOUND" ) );
        return;
    }

    //not in callback list
    if ( !m_callBackList.contains( id ) )
        return;

    QByteArray result = m_http->readAll();
    if ( result.isEmpty() )
        return;

    QDomDocument doc;
    if ( !doc.setContent( result ) )
        return;

    QDomElement e = doc.documentElement();

    if ( result.contains( "Fault" ) )
    {
        QString error = e.elementsByTagName( "message" ).item(0).toElement().text();
		if ( mobileResults.contains( error.toLatin1() ) )
			error = mobileResults.value( error.toLatin1() );
		labelError->setText( error );
		statusTimer->stop();
        return;
    }

    QMetaObject::invokeMethod( this, (const char *)m_callBackList.value( id ),
                               Q_ARG( QDomElement, e ) );
    m_callBackList.remove( id );
}

void MobileDialog::setSignatureInfo( const QString &city, const QString &state, const QString &zip,
										const QString &country, const QString &role, const QString &role2 )
{
	signature = QString(
			"<City xsi:type=\"xsd:String\">%1</City>"
			"<StateOrProvince xsi:type=\"xsd:String\">%2</StateOrProvince>"
			"<PostalCode xsi:type=\"xsd:String\">%3</PostalCode>"
			"<CountryName xsi:type=\"xsd:String\">%4</CountryName>"
			"<Role xsi:type=\"xsd:String\">%5 / %6</Role>"
			).arg( city ).arg( state ).arg( zip ).arg( country ).arg( role ).arg( role2 );
}

void MobileDialog::sign( const QByteArray &ssid, const QByteArray &cell )
{
	if ( !getFiles() )
		return;

	startTime.start();
	statusTimer->start( 1000 );

	labelError->setText( mobileResults.value( "START" ) );

	QByteArray format = m_doc->documentType() == digidoc::WDoc::BDocType ?
			"<Format xsi:type=\"xsd:String\">BDOC</Format>"
			"<Version xsi:type=\"xsd:String\">1.0</Version>" :
			"<Format xsi:type=\"xsd:String\">DIGIDOC-XML</Format>"
			"<Version xsi:type=\"xsd:String\">1.3</Version>";

	QByteArray message = "<IDCode xsi:type=\"xsd:String\">" + ssid + "</IDCode>"
			"<PhoneNo xsi:type=\"xsd:String\">" + cell + "</PhoneNo>"
			"<Language xsi:type=\"xsd:String\">EST</Language>"
			"<ServiceName xsi:type=\"xsd:String\">Testimine</ServiceName>"
			"<MessageToDisplay xsi:type=\"xsd:String\">DigiDoc3</MessageToDisplay>"
			+ signature.toLatin1() +
			"<SigningProfile xsi:type=\"xsd:String\"></SigningProfile>"
			+ files + format +
			"<SignatureID xsi:type=\"xsd:String\">S" + QByteArray::number( m_doc->signatures().size() ) + "</SignatureID>"
			"<MessagingMode xsi:type=\"xsd:String\">asynchClientServer</MessagingMode>"
			"<AsyncConfiguration xsi:type=\"xsd:int\">0</AsyncConfiguration>";
	int id = m_http->post( "/", insertBody( MobileCreateSignature, message ) );
	m_callBackList.insert( id, "startSessionResult" );
}

bool MobileDialog::getFiles()
{
	files = "<DataFiles xsi:type=\"m:DataFileDigestList\">";
	int i = 0;
	Q_FOREACH( digidoc::Document file, m_doc->documents() )
	{
		QByteArray digest;
		if ( m_doc->documentType() == digidoc::WDoc::BDocType )
		{
			std::auto_ptr<digidoc::Digest> calc = digidoc::Digest::create( URI_SHA1 );
			std::vector<unsigned char> d;
			try {
				 d = file.calcDigest( calc.get() );
			} catch( const digidoc::IOException &e ) {
				labelError->setText( QString::fromStdString( e.getMsg() ) );
				return false;
			}
			digest = QByteArray( (char*)&d[0], d.size() );
		} else
			digest = m_doc->getFileDigest( i ).left( 20 );

		QFileInfo f( QString::fromStdString( file.getPath() ) );
		files += "<DataFileDigest xsi:type=\"m:DataFileDigest\">"
			 + (m_doc->documentType() == digidoc::WDoc::BDocType ?
				"<Id xsi:type=\"xsd:String\">/" + f.fileName() + "</Id>" :
				"<Id xsi:type=\"xsd:String\">D" + QByteArray::number( i ) + "</Id>"	) +
			"<DigestType xsi:type=\"xsd:String\">sha1</DigestType>"
			"<DigestValue xsi:type=\"xsd:String\">" + digest.toBase64() + "</DigestValue>"
			"</DataFileDigest>";
		i++;
	}
	files += "</DataFiles>";
	return true;
}

void MobileDialog::startSessionResult( const QDomElement &element )
{
    sessionCode=element.elementsByTagName( "Sesscode" ).item(0).toElement().text().toInt();
    if ( sessionCode )
	{
		labelCode->setText( element.elementsByTagName( "ChallengeID" ).item(0).toElement().text() );
		if ( !m_timer )
		{
			m_timer = new QTimer( this );
			connect( m_timer, SIGNAL(timeout()), SLOT(getSignStatus()) );
		}
		m_timer->start( 5000 );
	} else
		labelError->setText( mobileResults.value( element.elementsByTagName( "message" ).item(0).toElement().text().toLatin1() ) );
}

void MobileDialog::getSignStatus()
{
        QByteArray message = "<Sesscode xsi:type=\"xsd:int\">" + QByteArray::number( sessionCode ) + "</Sesscode>"
                        "<WaitSignature xsi:type=\"xsd:boolean\">false</WaitSignature>";
		int id = m_http->post( "/", insertBody( GetMobileCreateSignatureStatus, message ) );
        m_callBackList.insert( id, "getSignStatusResult" );
}

void MobileDialog::getSignStatusResult( const QDomElement &element )
{
	QString status = element.elementsByTagName( "Status" ).item(0).toElement().text();
	labelError->setText( mobileResults.value( status.toLatin1() ) );

	//qDebug() << status << element.elementsByTagName( "Signature" ).item(0).toElement().text();

	if ( status != "REQUEST_OK" && status != "OUTSTANDING_TRANSACTION" )
	{
		m_timer->stop();
		statusTimer->stop();

		if ( status != "SIGNATURE" )
			return;

		QTemporaryFile file( QString( "%1%2XXXXXX.xml" )
							.arg( QDir::tempPath() ).arg( QDir::separator() ) );
		file.setAutoRemove( false );
		if ( file.open() )
		{
			fName = file.fileName();

			QByteArray data = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>";

			if ( m_doc->documentType() == digidoc::WDoc::DDocType )
				data += "<SignedDoc format=\"DIGIDOC-XML\" version=\"1.3\" xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\">";

			data += element.elementsByTagName( "Signature" ).item(0).toElement().text().toLatin1();

			if ( m_doc->documentType() == digidoc::WDoc::DDocType )
				data += "</SignedDoc>";
			file.write( data );
			file.close();
			close();
		}
	}
}

QByteArray MobileDialog::insertBody( MobileAction maction, const QByteArray &body ) const
{
    QByteArray action;
    switch ( maction )
    {
	case MobileCreateSignature: action="MobileCreateSignature"; break;
	case GetMobileCreateSignatureStatus: action="GetMobileCreateSignatureStatus"; break;
    }

    QByteArray message = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"> "
        "<SOAP-ENV:Body>"
                "<m:" + action + " xmlns:m=\"http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                    + body +
                "</m:" + action + ">"
        "</SOAP-ENV:Body>"
    "</SOAP-ENV:Envelope>";
    return message;
}

void MobileDialog::updateStatus()
{
	if ( statusTimer && statusTimer->isActive() && startTime.isValid() )
	{
		signProgressBar->setValue( startTime.elapsed() / 1000 );
		if ( signProgressBar->value() >= signProgressBar->maximum() )
		{
			m_timer->stop();
			statusTimer->stop();
			labelError->setText( mobileResults.value( "EXPIRED_TRANSACTION" ) );
		}
	}
}
