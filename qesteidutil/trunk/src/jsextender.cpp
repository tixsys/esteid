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

#include <QApplication>
#include <QDesktopServices>
#include <QDomDocument>
#include <QFileDialog>
#include <QInputDialog>
#include <QtWebKit>

#include "mainwindow.h"
#include "jsextender.h"
#include "sslConnect.h"
#include "Settings.h"
#include "SettingsDialog.h"

JsExtender::JsExtender( MainWindow *main )
:	QObject( main )
,	m_mainWindow( main )
,	m_loading( 0 )
{
	m_locale = Settings().value( "language" ).toString();
	if ( m_locale.isEmpty() )
		m_locale = QLocale::system().name().left( 2 );
	setLanguage( m_locale );

	connect( m_mainWindow->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(javaScriptWindowObjectCleared()));
}

JsExtender::~JsExtender()
{
	if ( m_loading )
		m_loading->deleteLater();
	if ( QFile::exists( m_tempFile ) )
		QFile::remove( m_tempFile );
}

void JsExtender::setLanguage( const QString &lang )
{
	m_locale = lang;
	if ( m_locale == "C" )
		m_locale = "en";
	Settings().setValue( "language", m_locale );
	m_mainWindow->retranslate( m_locale );
}

void JsExtender::registerObject( const QString &name, QObject *object )
{
    m_mainWindow->page()->mainFrame()->addToJavaScriptWindowObject( name, object );

    m_registeredObjects[name] = object;
}

void JsExtender::javaScriptWindowObjectCleared()
{
    for (QMap<QString, QObject *>::Iterator it = m_registeredObjects.begin(); it != m_registeredObjects.end(); ++it)
        m_mainWindow->page()->mainFrame()->addToJavaScriptWindowObject( it.key(), it.value() );

    m_mainWindow->page()->mainFrame()->addToJavaScriptWindowObject( "extender", this );
}

QVariant JsExtender::jsCall( const QString &function, int argument )
{
    return m_mainWindow->page()->mainFrame()->evaluateJavaScript(
		QString( "%1(%2)" ).arg( function ).arg( argument ) );
}

QVariant JsExtender::jsCall( const QString &function, const QString &argument )
{
    return m_mainWindow->page()->mainFrame()->evaluateJavaScript(
		QString( "%1(\"%2\")" ).arg( function ).arg( argument ) );
}

QVariant JsExtender::jsCall( const QString &function, const QString &argument, const QString &argument2 )
{
    return  m_mainWindow->page()->mainFrame()->evaluateJavaScript(
		QString( "%1(\"%2\",\"%3\")" ).arg( function ).arg( argument ).arg( argument2 ) );
}

void JsExtender::openUrl( const QString &url )
{ QDesktopServices::openUrl( QUrl( url ) ); }

QString JsExtender::checkPin()
{
	if ( !m_mainWindow->eidCard() || !m_mainWindow->eidCard()->m_card )
		throw std::runtime_error( "noCard" );
	if ( activeDocument.isEmpty() || activeDocument != m_mainWindow->eidCard()->getDocumentId() || pin.isEmpty() || 
		Settings().value( "sessionTime").toInt() == 0 || 
		!m_dateTime.isValid() || m_dateTime.addSecs( Settings().value( "sessionTime").toInt() * 60 ) < QDateTime::currentDateTime() )
	{
		activeDocument = m_mainWindow->eidCard()->getDocumentId();
		m_dateTime = QDateTime::currentDateTime();
		bool ok;
		pin = QInputDialog::getText( m_mainWindow, tr("Authentication"), tr("This action needs authentication\nEnter PIN1"), QLineEdit::Password, QString(), &ok );
		if( !ok || pin.size() < 4 )
		{
			pin = "";
			throw std::runtime_error( "" );
		}/* else if ( !m_mainWindow->eidCard()->validatePin1( pin ) ) {
			pin = "";
			throw std::runtime_error( "PIN1InvalidRetry" );
		}*/
	}
	QCoreApplication::processEvents();
	return pin;
}

QByteArray JsExtender::getUrl( SSLConnect::RequestType type, const QString &def )
{
	QString pin = checkPin();
	if ( pin.isEmpty() || pin.size() < 4 )
		throw std::runtime_error( "" );

	std::vector<unsigned char> buffer;

	SSLConnect *sslConnect = new SSLConnect( this );

	try {
		buffer = sslConnect->getUrl( pin.toStdString(), m_mainWindow->cardManager()->activeReaderNum(), type, def.toStdString() );
	} catch( const std::runtime_error &e ) {
		sslConnect->deleteLater();
		throw std::runtime_error( e );
	}
	
	sslConnect->deleteLater();

	return buffer.size() ? QByteArray( (char *)&buffer[0], buffer.size() ) : QByteArray();
}

void JsExtender::activateEmail( const QString &email )
{
	QByteArray buffer;
	try {
		buffer = getUrl( SSLConnect::ActivateEmails, email );
	} catch( std::runtime_error &e ) {
		jsCall( "setEmails", "forwardFailed", "" );
		jsCall( "handleError", e.what() );
		return;
	}
	if ( !buffer.size() )
	{
		jsCall( "setEmails", "forwardFailed", "" );
		return;
	}
	xml.clear();
	xml.addData( buffer );
	QString result = "forwardFailed";
	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() &&  xml.name() == "fault_code" )
		{
			result = xml.readElementText();
			break;
		}
	}
	jsCall( "setEmailActivate", result );
}

void JsExtender::loadEmails()
{
	QByteArray buffer;
	try {
		buffer = getUrl( SSLConnect::EmailInfo, "" );
	} catch( std::runtime_error &e ) {
		jsCall( "setEmails", "loadFailed", "" );
		jsCall( "handleError", e.what() );
		return;
	}

	if ( !buffer.size() )
	{
		jsCall( "setEmails", "loadFailed", "" );
		return;
	}
	xml.clear();
	xml.addData( buffer );
	bool error = false, notActivated = false;
	QString result = "loadFailed", emails;
	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == "fault_code" )
			{
				result = xml.readElementText();
				continue;
			}
			if ( xml.name() == "ametlik_aadress" )
				emails += readEmailAddresses();
		}
	}
	if ( emails.length() > 4 )
		emails = emails.right( emails.length() - 4 );
	jsCall( "setEmails", result, QByteArray( QUrl::toPercentEncoding( emails, "<BR>@()- " ) ).constData() );
}

QString JsExtender::readEmailAddresses()
{
	Q_ASSERT( xml.isStartElement() && xml.name() == "ametlik_aadress" );

	QString emails;

	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == "epost" )
				emails += "<BR>" + xml.readElementText();
			else if ( xml.name() == "suunamine" )
				emails += readForwards();
		}
	}
	return emails;
}

QString JsExtender::readForwards()
{
	Q_ASSERT( xml.isStartElement() && xml.name() == "suunamine" );

	bool emailActive = false, forwardActive = false;
	QString email;

	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isEndElement() )
			break;
		if ( xml.isStartElement() )
		{
			if ( xml.name() == "epost" )
				email = xml.readElementText();
			else if ( xml.name() == "aktiivne" && xml.readElementText() == "true" )
				emailActive = true;
			else if ( xml.name() == "aktiiveeritud" && xml.readElementText() == "true" )
				forwardActive = true;
		}
	}
	return (emailActive && forwardActive ) ? tr( " - %1 (active)" ).arg( email ) : tr( " - %1 (not active)" ).arg( email );
}

void JsExtender::loadPicture()
{
	QString result = "loadPicFailed";
	QByteArray buffer;
	try {
		buffer = getUrl( SSLConnect::PictureInfo, "" );
	} catch( std::runtime_error &e ) {
		jsCall( "setPicture", "", result );
		jsCall( "handleError", e.what() );
		return;
	}
	if ( !buffer.size() )
	{
		jsCall( "setPicture", "", result );
		return;
	}

	QPixmap pix;
	if ( pix.loadFromData( buffer ) )
	{
		QTemporaryFile file( QString( "%1%2XXXXXX.jpg" )
			.arg( QDir::tempPath() ).arg( QDir::separator() ) );
		file.setAutoRemove( false );
		if ( file.open() )
		{
			m_tempFile = file.fileName();
			if ( pix.save( &file ) )
			{
#ifdef WIN32
				QString url = QString( "file:///" ).append( m_tempFile );
#else
				QString url = QString( "file://" ).append( m_tempFile );
#endif
				jsCall( "setPicture", url, "" );
				return;
			}
		}
		jsCall( "setPicture", "", QString( "loadPicFailed3|%1" ).arg( file.errorString() ) );
	} else { //probably got xml error string
		QString result2 = "loadPicFailed2";
		xml.clear();
		xml.addData( buffer );		
		while ( !xml.atEnd() )
		{
			xml.readNext();
			if ( xml.isStartElement() && xml.name() == "fault_code" )
			{
				result2 = xml.readElementText();
				break;
			}
		}
		jsCall( "setPicture", "", result2 );
	}
}

void JsExtender::savePicture()
{
	if ( !QFile::exists( m_tempFile ) )
	{
		jsCall( "handleError", "savePicFailed" );
		return;
	}
	QString pFile = QDesktopServices::storageLocation( QDesktopServices::PicturesLocation );
	if ( m_mainWindow->eidCard() )
		pFile += QString( "%1%2.jpg" ).arg( QDir::separator() ).arg( m_mainWindow->eidCard()->getId() );
	QString file = QFileDialog::getSaveFileName( m_mainWindow, tr( "Save picture" ), pFile, tr( "JPEG (*.jpg *.jpeg);;PNG (*.png);;TIFF (*.tif *.tiff);;24-bit Bitmap (*.bmp)" ) );
	if( file.isEmpty() )
		return;
	QString ext = QFileInfo( file ).suffix();
	if( ext != "png" && ext != "jpg" && ext != "jpeg" && ext != "tiff" && ext != "bmp" )
		file.append( ".jpg" );
	QPixmap pix;
	if ( !pix.load( m_tempFile ) )
	{
		jsCall( "handleError", "savePicFailed" );
		return;
	}
	pix.save( file );
}

void JsExtender::getMidStatus()
{
	QString result = "mobileFailed";
	QByteArray buffer;

	QString data = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
						"<SOAP-ENV:Body>"
						"<m:GetMIDTokens xmlns:m=\"urn:GetMIDTokens\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"/>"
						"</SOAP-ENV:Body>"
						"</SOAP-ENV:Envelope>";
	QString header = "POST /id/midstatusinfolive/ HTTP/1.1\r\n"
					 "Host: " + QString(SK) + "\r\n"
					 "Content-Type: text/xml\r\n"
					 "Content-Length: " + QString::number( data.size() ) + "\r\n"
					 "SOAPAction: \"\"\r\n"
					 "Connection: close\r\n\r\n";
	try {
		buffer = getUrl( SSLConnect::MobileInfo, header + data );
	} catch( std::runtime_error &e ) {
		jsCall( "setMobile", "", result );
		jsCall( "handleError", e.what() );
		return;
	}
	if ( !buffer.size() )
	{
		jsCall( "setMobile", "", result );
		return;
	}
	//qDebug() << buffer;
	if ( !buffer.isEmpty() )
	{
		QDomDocument doc;
		if ( !doc.setContent( buffer ) )
		{
			jsCall( "handleError", result );
			return;
		}
		QDomElement e = doc.documentElement();
		if ( !e.elementsByTagName( "ResponseStatus" ).size() )
		{
			jsCall( "handleError", result );
			return;
		}
		MobileResult mRes = (MobileResult)e.elementsByTagName( "ResponseStatus" ).item(0).toElement().text().toInt();
		QString mResString;
		switch( mRes )
		{
			case NoCert: mResString = "mobileNoCert"; break;
			case NotActive: mResString = "mobileNotActive"; break;
			case NoIDCert: mResString = "noIDCert"; break;
			case InternalError: mResString = "mobileInternalError"; break;
			case InterfaceNotReady: mResString = "mobileInterfaceNotReady"; break;
			case OK:
			default: break;
		}
		if ( !mResString.isEmpty() )
		{
			jsCall( "handleError", mResString );
			return;
		}
		mResString = QString( "%1;%2;%3;%4" )
						.arg( e.elementsByTagName( "MSISDN" ).item(0).toElement().text() )
						.arg( e.elementsByTagName( "Operator" ).item(0).toElement().text() )
						.arg( e.elementsByTagName( "Status" ).item(0).toElement().text() )
						.arg( e.elementsByTagName( "URL" ).item(0).toElement().text() );
		jsCall( "setMobile", mResString );
	}

	/*
	QHttpRequestHeader header( "POST", "/id/midstatusinfo/", 1, 1 );
	header.setValue( "host", "www.sk.ee" );
	header.setContentType( "text/xml" );
	header.setContentLength( data.length() );
	m_http.setHost( "www.sk.ee" );
	m_http.request( header, data );
	connect( &m_http, SIGNAL(requestFinished(int, bool)), SLOT(httpRequestFinished(int, bool)) );
	*/
}

void JsExtender::httpRequestFinished( int, bool error )
{
	if ( error)
		qDebug() << "Download failed: " << m_http.errorString();

	QByteArray result = m_http.readAll();
}

void JsExtender::showSettings()
{ (new SettingsDialog( m_mainWindow ) )->show(); }

void JsExtender::showLoading( const QString &str )
{
	bool wide = (str.size() > 20);
	if ( !m_loading )
	{
		m_loading = new QLabel( m_mainWindow );
		m_loading->setStyleSheet( "background-color: rgba(255,255,255,200); border: 1px solid #cddbeb; border-radius: 5px;"
									"color: #509b00; font-weight: bold; font-family: Arial; font-size: 18px;" );
		m_loading->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	}
	m_loading->setFixedSize( wide ? 300 : 250, 100 );
	m_loading->move( wide ? 155 : 180, 305 );
	m_loading->setText( str );
	m_loading->show();
	QCoreApplication::processEvents();
}

void JsExtender::closeLoading()
{
	if ( m_loading )
		m_loading->close();
}
