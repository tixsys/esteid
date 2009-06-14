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

#include <libdigidoc++/Document.h>
#include <libdigidoc++/Exception.h>
#include <libdigidoc++/crypto/Digest.h>
#include <libdigidoc++/WDoc.h>

#include <QDebug>
#include <QDir>
#include <QTemporaryFile>

MobileDialog::MobileDialog( DigiDoc *doc, QWidget *parent )
:   QDialog( parent )
,   m_doc( doc )
,   m_timer( 0 )
,   sessionCode( 0 )
{
    setupUi( this );

    m_http = new QHttp( this );
    connect( m_http, SIGNAL(requestFinished(int, bool)), SLOT(httpRequestFinished(int, bool)) );
    connect( m_http, SIGNAL(sslErrors(const QList<QSslError> &)), SLOT(sslErrors(const QList<QSslError> &)) );
    m_http->setHost( "digidocservice.sk.ee", QHttp::ConnectionModeHttps );
   //m_http->setHost( "www.openxades.org", QHttp::ConnectionModeHttps, 8443 );
}

void MobileDialog::sslErrors(const QList<QSslError> &)
{ m_http->ignoreSslErrors(); }

void MobileDialog::httpRequestFinished( int id, bool error )
{
    if ( error)
    {
        qDebug() << "Download failed: " << m_http->errorString();
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
		labelError->setText( error );
        qDebug() << result;
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
	QByteArray message = "<IDCode xsi:type=\"xsd:String\">" + ssid + "</IDCode>"
			"<PhoneNo xsi:type=\"xsd:String\">" + cell + "</PhoneNo>"
			"<Language xsi:type=\"xsd:String\">EST</Language>"
			"<ServiceName xsi:type=\"xsd:String\">Testimine</ServiceName>"
			"<MessageToDisplay xsi:type=\"xsd:String\">DigiDoc3</MessageToDisplay>"
			+ signature.toLatin1() +
			"<SigningProfile xsi:type=\"xsd:String\"></SigningProfile>"
			+ getFiles() +
			"<Format xsi:type=\"xsd:String\">DIGIDOC-XML</Format>"
			"<Version xsi:type=\"xsd:String\">1.3</Version>"
			"<SignatureID xsi:type=\"xsd:String\">S" + QByteArray::number( m_doc->signatures().size() ) + "</SignatureID>"
			"<MessagingMode xsi:type=\"xsd:String\">asynchClientServer</MessagingMode>"
			"<AsyncConfiguration xsi:type=\"xsd:int\">0</AsyncConfiguration>";
	int id = m_http->post( "/", insertBody( MobileCreateSignature, message ) );
	m_callBackList.insert( id, "startSessionResult" );
}

QByteArray MobileDialog::getFiles() const
{
	QByteArray message = "<DataFiles xsi:type=\"m:DataFileDigestList\">";
	int i = 0;
	Q_FOREACH( digidoc::Document file, m_doc->documents() )
	{
		std::auto_ptr<digidoc::Digest> calc = digidoc::Digest::create( URI_SHA1 );
		std::vector<unsigned char> d = file.calcDigest( calc.get() );
		QByteArray digest( (char*)&d[0], d.size() );
		message += "<DataFileDigest xsi:type=\"m:DataFileDigest\">"
			"<Id xsi:type=\"xsd:String\">D" + QByteArray::number( i ) + "</Id>"
			"<DigestType xsi:type=\"xsd:String\">sha1</DigestType>"
			"<DigestValue xsi:type=\"xsd:String\">" + digest.toBase64() + "</DigestValue>"
			"</DataFileDigest>";
		i++;
	}
	message += "</DataFiles>";
	return message;
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
	}
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
	labelError->setText( status );

	//qDebug() << status << element.elementsByTagName( "Signature" ).item(0).toElement().text();

	if ( status == "SIGNATURE" )
	{
        	m_timer->stop();

		QTemporaryFile file( QString( "%1%2XXXXXX.xml" )
							.arg( QDir::tempPath() ).arg( QDir::separator() ) );
		file.setAutoRemove( false );
		if ( file.open() )
		{
			fName = file.fileName();
			QByteArray data = element.elementsByTagName( "Signature" ).item(0).toElement().text().toLatin1();
			
			QRegExp rx( "<Exponent>(.*)</Exponent>" );
			rx.indexIn( data );
			data.replace( rx.cap(1) + "<SignedProperties", "<SignedProperties" )
				.replace( "<Exponent>AQAB","<Exponent>F4O3")
				.replace( "xmlns=\"http://www.w3.org/2000/09/xmldsig#\"", "xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\"")
				.replace( "v1.1.1", "v1.3.2" )
				.replace( "<ResponderID>", "<ResponderID><ByName>" )
				.replace( "</ResponderID>", "</ByName></ResponderID>" )
				.replace( "<Signature ", "<ds:Signature xmlns=\"http://uri.etsi.org/01903/v1.3.2#\" " )
				.replace( "</Signature>", "</ds:Signature>" )
				.replace( "<SignedInfo", "<ds:SignedInfo" )
				.replace( "</SignedInfo>", "</ds:SignedInfo>" )
				.replace( "<CanonicalizationMethod", "<ds:CanonicalizationMethod" )
				.replace( "</CanonicalizationMethod>", "</ds:CanonicalizationMethod>" )
				.replace( "<SignatureMethod", "<ds:SignatureMethod" )
				.replace( "</SignatureMethod>", "</ds:SignatureMethod>" )
				.replace( "<Reference", "<ds:Reference" )
				.replace( "</Reference>", "</ds:Reference>" )
				.replace( "<DigestMethod", "<ds:DigestMethod" )
				.replace( "</DigestMethod>", "</ds:DigestMethod>" )
				.replace( "<DigestValue", "<ds:DigestValue" )
				.replace( "</DigestValue>", "</ds:DigestValue>" )
				.replace( "<SignatureValue", "<ds:SignatureValue" )
				.replace( "</SignatureValue>", "</ds:SignatureValue>" )
				.replace( "<KeyInfo", "<ds:KeyInfo" )
				.replace( "</KeyInfo>", "</ds:KeyInfo>" )
				.replace( "<KeyValue", "<ds:KeyValue" )
				.replace( "</KeyValue>", "</ds:KeyValue>" )
				.replace( "<RSAKeyValue", "<ds:RSAKeyValue" )
				.replace( "</RSAKeyValue>", "</ds:RSAKeyValue>" )
				.replace( "<Modulus", "<ds:Modulus" )
				.replace( "</Modulus>", "</ds:Modulus>" )
				.replace( "<Exponent", "<ds:Exponent" )
				.replace( "</Exponent>", "</ds:Exponent>" )
				.replace( "<X509Data", "<ds:X509Data" )
				.replace( "</X509Data>", "</ds:X509Data>" )
				.replace( "<X509Certificate", "<ds:X509Certificate" )
				.replace( "</X509Certificate>", "</ds:X509Certificate>" )
				.replace( "<Object>", "<ds:Object>" )
				.replace( "</Object>", "</ds:Object>" )
				.replace( "<X509IssuerName", "<ds:X509IssuerName" )
				.replace( "</X509IssuerName>", "</ds:X509IssuerName>" )
				.replace( "<X509SerialNumber", "<ds:X509SerialNumber" )
				.replace( "</X509SerialNumber>", "</ds:X509SerialNumber>" );

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
