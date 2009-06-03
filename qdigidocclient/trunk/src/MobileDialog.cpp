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
#include <digidoc/Document.h>
#include <digidoc/crypto/Digest.h>

#include <QDebug>
#include <QFileInfo>

MobileDialog::MobileDialog( DigiDoc *doc, QWidget *parent )
:   QDialog( parent )
,   m_doc( doc )
,   sessionCode( 0 )
,   m_timer( 0 )
{
    setupUi( this );

    m_http = new QHttp( this );
    connect( m_http, SIGNAL(requestFinished(int, bool)), SLOT(httpRequestFinished(int, bool)) );
    connect( m_http, SIGNAL(sslErrors(const QList<QSslError> &)), SLOT(sslErrors(const QList<QSslError> &)) );
    m_http->setHost( "digidocservice.sk.ee", QHttp::ConnectionModeHttps );
}

void MobileDialog::on_buttonNext_clicked()
{
    switch( pages->currentIndex() )
    {
        case Start:
            startSession();
            pages->setCurrentIndex( Challenge );
            break;
        case Challenge: break;
    }
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
        qDebug() << result;
        return;
    }

    QMetaObject::invokeMethod( this, (const char *)m_callBackList.value( id ),
                               Q_ARG( QDomElement, e ) );
    m_callBackList.remove( id );
}

void MobileDialog::startSession()
{
    QByteArray message = "<SigDocXML xsi:type=\"xsd:String\"></SigDocXML>"
                        "<bHoldSession xsi:type=\"xsd:boolean\">true</bHoldSession>"
                        "<datafile xsi:type=\"m:DataFileData\"></datafile>";
    int id = m_http->post( "/", insertBody( StartSession, message ) );
    m_callBackList.insert( id, "startSessionResult" );
}

void MobileDialog::startSessionResult( const QDomElement &element )
{
    sessionCode=element.elementsByTagName( "Sesscode" ).item(0).toElement().text().toInt();
    if ( sessionCode )
        createSignedDoc();
}

void MobileDialog::createSignedDoc()
{
        QByteArray message = "<Sesscode xsi:type=\"xsd:int\">" + QByteArray::number( sessionCode ) + "</Sesscode>"
                        "<Format xsi:type=\"xsd:String\">DIGIDOC-XML</Format>"
                        "<Version xsi:type=\"xsd:String\">1.3</Version>";
        int id = m_http->post( "/", insertBody( CreateSignedDoc, message ) );
        m_callBackList.insert( id, "createSignedDocResult" );
}

void MobileDialog::createSignedDocResult( const QDomElement &element )
{ addFiles(); }

void MobileDialog::addFiles()
{
    addedFiles = 0;
    Q_FOREACH( digidoc::Document file, m_doc->documents() )
    {
        QFileInfo info( QString::fromUtf8( file.getPath().data() ) );
        std::auto_ptr<digidoc::Digest> calc = digidoc::Digest::create( digidoc::Digest::URI_SHA1 );
        std::vector<unsigned char> d = file.calcDigest( calc.get() );
        QByteArray digest( (char*)&d[0], d.size() );
        QByteArray message = "<Sesscode xsi:type=\"xsd:int\">" + QByteArray::number( sessionCode ) + "</Sesscode>"
                        "<FileName xsi:type=\"xsd:String\">" + info.fileName().toLatin1() + "</FileName>"
                        "<MimeType xsi:type=\"xsd:String\">" + QString::fromStdString(file.getMediaType()).toLatin1() + "</MimeType>"
                        "<ContentType xsi:type=\"xsd:String\">HASHCODE</ContentType>"
                        "<DigestType xsi:type=\"xsd:String\">sha1</DigestType>"
                        "<DigestValue xsi:type=\"xsd:String\">" + digest.toBase64() + "</DigestValue>"
                        "<Size xsi:type=\"xsd:int\">" + QByteArray::number( info.size() ) + "</Size>";
        int id = m_http->post( "/", insertBody( AddDataFile, message ) );
        m_callBackList.insert( id, "addFilesResult" );
    }
}

void MobileDialog::addFilesResult( const QDomElement &element )
{
    addedFiles++;
    //all documents added
    if ( addedFiles = m_doc->documents().size() )
        startSign();
}

void MobileDialog::startSign()
{
    QByteArray message = "<Sesscode xsi:type=\"xsd:int\">" + QByteArray::number( sessionCode ) + "</Sesscode>"
                        "<SignerIDCode xsi:type=\"xsd:String\">38108290256</SignerIDCode>"
                        "<SignerPhoneNo xsi:type=\"xsd:String\">+37253492310</SignerPhoneNo>"
                        "<ServiceName xsi:type=\"xsd:String\">DigiDoc Client</ServiceName>"
                        "<AdditionalDataToBeDisplayed xsi:type=\"xsd:String\">DigiDoc3</AdditionalDataToBeDisplayed>"
                        "<Language xsi:type=\"xsd:String\">EST</Language>"
                        "<Role xsi:type=\"xsd:String\"></Role>"
                        "<City xsi:type=\"xsd:String\"></City>"
                        "<StateOrProvince xsi:type=\"xsd:String\"></StateOrProvince>"
                        "<PostalCode xsi:type=\"xsd:String\"></PostalCode>"
                        "<CountryName xsi:type=\"xsd:String\"></CountryName>"
                        "<SigningProfile xsi:type=\"xsd:String\"></SigningProfile>"
                        "<MessagingMode xsi:type=\"xsd:String\">asynchClientServer</MessagingMode>"
                        "<AsyncConfiguration xsi:type=\"xsd:int\">0</AsyncConfiguration>"
                        "<ReturnDocInfo xsi:type=\"xsd:boolean\">false</ReturnDocInfo>"
                        "<ReturnDocData xsi:type=\"xsd:boolean\">false</ReturnDocData>";
    int id = m_http->post( "/", insertBody( StartSign, message ) );
    m_callBackList.insert( id, "startSignResult" );
}

void MobileDialog::startSignResult( const QDomElement &element )
{
    labelCode->setText( element.elementsByTagName( "ChallengeID" ).item(0).toElement().text() );
    if ( !m_timer )
    {
        m_timer = new QTimer( this );
        connect( m_timer, SIGNAL(timeout()), SLOT(getSignStatus()) );
    }
    m_timer->start( 5000 );
}

void MobileDialog::getSignStatus()
{
        QByteArray message = "<Sesscode xsi:type=\"xsd:int\">" + QByteArray::number( sessionCode ) + "</Sesscode>"
                        "<ReturnDocInfo xsi:type=\"xsd:boolean\">true</ReturnDocInfo>"
                        "<WaitSignature xsi:type=\"xsd:boolean\">false</WaitSignature>";
        int id = m_http->post( "/", insertBody( SignStatus, message ) );
        m_callBackList.insert( id, "getSignStatusResult" );
}

void MobileDialog::getSignStatusResult( const QDomElement &element )
{
    labelError->setText( element.elementsByTagName( "StatusCode" ).item(0).toElement().text() );

    qDebug() << element.elementsByTagName( "SignedDocInfo" ).item(0).toElement().text();
}

QByteArray MobileDialog::insertBody( MobileAction maction, const QByteArray &body ) const
{
    QByteArray action;
    switch ( maction )
    {
        case StartSession: action="StartSession"; break;
        case CreateSignedDoc: action="CreateSignedDoc"; break;
        case AddDataFile: action="AddDataFile"; break;
        case StartSign: action="MobileSign"; break;
        case SignStatus: action="GetStatusInfo"; break;
        case CloseSession: action="CloseSession"; break;
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
