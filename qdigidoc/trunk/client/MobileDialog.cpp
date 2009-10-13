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
#include <QSslKey>
#include <QSslSocket>
#include <QTemporaryFile>

MobileDialog::MobileDialog( DigiDoc *doc, QWidget *parent )
:	QDialog( parent )
,	m_doc( doc )
,	m_http( new QHttp( this ) )
,	m_timer( new QTimer( this ) )
,	statusTimer( new QTimer( this ) )
,	sessionCode( 0 )
{
	setupUi( this );

	connect( m_http, SIGNAL(requestFinished(int, bool)), SLOT(httpRequestFinished(int, bool)) );
	connect( m_http, SIGNAL(sslErrors(const QList<QSslError> &)), SLOT(sslErrors(const QList<QSslError> &)) );
	connect( m_timer, SIGNAL(timeout()), SLOT(getSignStatus()) );
	connect( statusTimer, SIGNAL(timeout()), SLOT(updateStatus()) );

	Settings s;
	s.beginGroup( "Client" );

	QString certFile = m_doc->getConfValue( DigiDoc::PKCS12Cert, s.value( "pkcs12Cert" ) );
	QString certPass = m_doc->getConfValue( DigiDoc::PKCS12Pass, s.value( "pkcs12Pass" ) );
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

	m_http->setProxy(
		s.value( "proxyHost" ).toString(),
		s.value( "proxyPort" ).toInt(),
		s.value( "proxyUser" ).toString(),
		s.value( "proxyPass" ).toString() );

	if ( m_doc->documentType() == digidoc::WDoc::BDocType )
		//m_http->setHost( "www.openxades.org", QHttp::ConnectionModeHttps, 8443 );
		m_http->setHost( "www.sk.ee", QHttp::ConnectionModeHttps, 8097 );
	else
		m_http->setHost( "digidocservice.sk.ee", QHttp::ConnectionModeHttps );

	lang["et"] = "EST";
	lang["en"] = "ENG";
	lang["ru"] = "RUS";

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
}

QString MobileDialog::escapeChars( const QString &in ) const
{
	QString out;
	out.reserve( in.size() );
	for( QString::ConstIterator i = in.constBegin(); i != in.constEnd(); ++i )
	{
		if( *i == '\'' ) out += "&apos;";
		else if( *i == '\"' ) out += "&quot;";
		else if( *i == '<' ) out += "&lt;";
		else if( *i == '>' ) out += "&gt;";
		else if( *i == '&' ) out += "&amp;";
		else out += *i;
	}
	return out;
}

void MobileDialog::sslErrors(const QList<QSslError> &)
{ m_http->ignoreSslErrors(); }

void MobileDialog::httpRequestFinished( int id, bool error )
{
	if ( error )
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
	if ( !doc.setContent( QString::fromUtf8( result ) ) )
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
	QStringList roles = QStringList() << role << role2;
	roles.removeAll( "" );
	signature = QString(
		"<City xsi:type=\"xsd:String\">%1</City>"
		"<StateOrProvince xsi:type=\"xsd:String\">%2</StateOrProvince>"
		"<PostalCode xsi:type=\"xsd:String\">%3</PostalCode>"
		"<CountryName xsi:type=\"xsd:String\">%4</CountryName>"
		"<Role xsi:type=\"xsd:String\">%5</Role>")
		.arg( escapeChars( city ) )
		.arg( escapeChars( state ) )
		.arg( escapeChars( zip ) )
		.arg( escapeChars( country ) )
		.arg( escapeChars( roles.join(" / ") ) );
}

void MobileDialog::sign( const QString &ssid, const QString &cell )
{
	if ( !getFiles() )
		return;

	startTime.start();
	statusTimer->start( 1000 );

	labelError->setText( mobileResults.value( "START" ) );

	QString message = QString(
		"<IDCode xsi:type=\"xsd:String\">%1</IDCode>"
		"<PhoneNo xsi:type=\"xsd:String\">%2</PhoneNo>"
		"<Language xsi:type=\"xsd:String\">%3</Language>"
		"<ServiceName xsi:type=\"xsd:String\">Testimine</ServiceName>"
		"<MessageToDisplay xsi:type=\"xsd:String\">DigiDoc3</MessageToDisplay>"
		"%4"
		"<SigningProfile xsi:type=\"xsd:String\"></SigningProfile>"
		"%5"
		"<Format xsi:type=\"xsd:String\">%6</Format>"
		"<Version xsi:type=\"xsd:String\">%7</Version>"
		"<SignatureID xsi:type=\"xsd:String\">S%8</SignatureID>"
		"<MessagingMode xsi:type=\"xsd:String\">asynchClientServer</MessagingMode>"
		"<AsyncConfiguration xsi:type=\"xsd:int\">0</AsyncConfiguration>" )
		.arg( escapeChars( ssid ) )
		.arg( escapeChars( cell ) )
		.arg( lang.value( Settings().value("Main/Language", "et" ).toByteArray(), "EST" ) )
		.arg( signature )
		.arg( files )
		.arg( m_doc->documentType() == digidoc::WDoc::BDocType ? "BDOC" : "DIGIDOC-XML" )
		.arg( m_doc->documentType() == digidoc::WDoc::BDocType ? "1.0" : "1.3" )
		.arg( m_doc->signatures().size() );
	int id = m_http->post( "/", insertBody( MobileCreateSignature, message ).toUtf8() );
	m_callBackList.insert( id, "startSessionResult" );
}

bool MobileDialog::getFiles()
{
	files = "<DataFiles xsi:type=\"m:DataFileDigestList\">";
	int i = 0;
	Q_FOREACH( digidoc::Document file, m_doc->documents() )
	{
		QByteArray digest;
		QString name = "sha1";
		if ( m_doc->documentType() == digidoc::WDoc::BDocType )
		{
			std::auto_ptr<digidoc::Digest> calc = digidoc::Digest::create();
			std::vector<unsigned char> d;
			try {
				 d = file.calcDigest( calc.get() );
			} catch( const digidoc::IOException &e ) {
				labelError->setText( QString::fromStdString( e.getMsg() ) );
				return false;
			}
			digest = QByteArray( (char*)&d[0], d.size() );
			name = QString::fromStdString( calc->getName() );
		} else
			digest = m_doc->getFileDigest( i ).left( 20 );

		QFileInfo f( QString::fromStdString( file.getPath() ) );
		files += QString(
			"<DataFileDigest xsi:type=\"m:DataFileDigest\">"
			"<Id xsi:type=\"xsd:String\">%1</Id>"
			"<DigestType xsi:type=\"xsd:String\">%2</DigestType>"
			"<DigestValue xsi:type=\"xsd:String\">%3</DigestValue>"
			"</DataFileDigest>" )
			.arg( m_doc->documentType() == digidoc::WDoc::BDocType ?
				"/" + f.fileName() : "D" + QString::number( i ) )
			.arg( escapeChars( name ) ).arg( digest.toBase64().constData() );
		i++;
	}
	files += "</DataFiles>";
	return true;
}

void MobileDialog::startSessionResult( const QDomElement &element )
{
	sessionCode = element.elementsByTagName( "Sesscode" ).item(0).toElement().text().toInt();
	if ( sessionCode )
	{
		code->setText( tr("Control code: %1")
			.arg( element.elementsByTagName( "ChallengeID" ).item(0).toElement().text() ) );
		m_timer->start( 5000 );
	} else
		labelError->setText( mobileResults.value( element.elementsByTagName( "message" ).item(0).toElement().text().toLatin1() ) );
}

void MobileDialog::getSignStatus()
{
	QString message = QString(
		"<Sesscode xsi:type=\"xsd:int\">%1</Sesscode>"
		"<WaitSignature xsi:type=\"xsd:boolean\">false</WaitSignature>" )
		.arg( sessionCode );
	int id = m_http->post( "/", insertBody( GetMobileCreateSignatureStatus, message ).toUtf8() );
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
			file.write( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n" );
			if ( m_doc->documentType() == digidoc::WDoc::DDocType )
				file.write( "<SignedDoc format=\"DIGIDOC-XML\" version=\"1.3\" xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\">\n" );
			file.write( element.elementsByTagName( "Signature" ).item(0).toElement().text().toUtf8() );
			if ( m_doc->documentType() == digidoc::WDoc::DDocType )
				file.write( "</SignedDoc>" );
			file.close();
			close();
		}
	}
}

QString MobileDialog::insertBody( MobileAction maction, const QString &body ) const
{
	QString action;
	switch ( maction )
	{
	case MobileCreateSignature: action="MobileCreateSignature"; break;
	case GetMobileCreateSignatureStatus: action="GetMobileCreateSignatureStatus"; break;
	}

	return QString(
		"<SOAP-ENV:Envelope"
		"	xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\""
		"	xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\""
		"	xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
		"	xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
		"<SOAP-ENV:Body>"
		"<m:%1"
		"	xmlns:m=\"http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl\""
		"	SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">%2</m:%1>"
		"</SOAP-ENV:Body>"
		"</SOAP-ENV:Envelope>" ).arg( action ).arg( body );
}

void MobileDialog::updateStatus()
{
	if ( !statusTimer->isActive() || !startTime.isValid() )
		return;

	signProgressBar->setValue( startTime.elapsed() / 1000 );
	if ( signProgressBar->value() >= signProgressBar->maximum() )
	{
		m_timer->stop();
		statusTimer->stop();
		labelError->setText( mobileResults.value( "EXPIRED_TRANSACTION" ) );
	}
}
