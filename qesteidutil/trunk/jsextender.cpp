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
#include <QInputDialog>
#include <QtWebKit>

#include "mainwindow.h"
#include "jsextender.h"
#include "Settings.h"
#include "SettingsDialog.h"

JsExtender::JsExtender( MainWindow *main )
:	QObject( main )
,	m_mainWindow( main )
,	m_loading( 0 )
{
	m_locale = Settings().value( "language" ).toString();
	if ( m_locale.isEmpty() )
		setLanguage( QLocale::system().name().left( 2 ) );

	connect( m_mainWindow->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(javaScriptWindowObjectCleared()));
}

JsExtender::~JsExtender()
{
	if ( QFile::exists( m_tempFile ) )
		QFile::remove( m_tempFile );
}

void JsExtender::setLanguage( const QString &lang )
{
	m_locale = lang;
	if ( m_locale == "C" )
		m_locale = "en";
	Settings().setValue( "language", m_locale );
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
	if ( !m_mainWindow->cardManager()->ssl() )
		throw std::runtime_error( "" );

	QString pin;
	if ( !m_mainWindow->cardManager()->ssl()->isLoaded() || Settings().value( "sessionTime").toInt() == 0 || 
		!m_dateTime.isValid() || m_dateTime.addSecs( Settings().value( "sessionTime").toInt() * 60 ) < QDateTime::currentDateTime() )
	{
		m_dateTime = QDateTime::currentDateTime();
		bool ok;
		pin = QInputDialog::getText( m_mainWindow, tr("Isikutuvastus"), tr("Sisesta PIN1"), QLineEdit::Password, QString(), &ok );
		if( !ok )
			throw std::runtime_error( "" );
		else if ( !m_mainWindow->eidCard()->validatePin1( pin ) )
			throw std::runtime_error( "PIN1InvalidRetry" );
	}
	QCoreApplication::processEvents();
	return pin;
}

void JsExtender::activateEmail( const QString &email )
{
	std::vector<unsigned char> buffer;
	try {
		buffer = m_mainWindow->cardManager()->ssl()->getUrl( "activateEmail", checkPin().toStdString(), email.toStdString() );
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
	xml.addData( QByteArray( (char *)&buffer[0], buffer.size() ) );
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
	std::vector<unsigned char> buffer;
	try {
		buffer = m_mainWindow->cardManager()->ssl()->getUrl( "emails", checkPin().toStdString() );
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
	xml.addData( QByteArray( (char *)&buffer[0], buffer.size() ) );
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
	return (emailActive && forwardActive ) ? tr( " - %1 (aktiivne)" ).arg( email ) : tr( " - %1 (mitteaktiivne)" ).arg( email );
}

void JsExtender::loadPicture()
{
	QString result = "loadPicFailed";
	std::vector<unsigned char> buffer;
	try {
		buffer = m_mainWindow->cardManager()->ssl()->getUrl( "picture", checkPin().toStdString() );
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
	if ( pix.loadFromData( (uchar *)&buffer[0], buffer.size(), "jpeg" ) )
	{
		QTemporaryFile file( QString( "%1%2XXXXXX.jpg" )
			.arg( QDir::tempPath() ).arg( QDir::separator() ) );
		file.setAutoRemove( false );
		if ( file.open() )
		{
			m_tempFile = file.fileName();
			pix = pix.scaled( 90, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
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
	} else { //probably got xml error string
		xml.clear();
		xml.addData( QByteArray( (char *)&buffer[0], buffer.size() ) );		
		while ( !xml.atEnd() )
		{
			xml.readNext();
			if ( xml.isStartElement() && xml.name() == "fault_code" )
			{
				result = xml.readElementText();
				break;
			}
		}
	}
	jsCall( "setPicture", "", result );
}

void JsExtender::showLoading( const QString &str )
{
	if ( !m_loading )
	{
		m_loading = new QLabel( m_mainWindow );
		m_loading->setStyleSheet( "background-color: rgba(255,255,255,180); border: 1px solid #cddbeb; border-radius: 3px;"
									"color: #509b00; font-weight: bold; font-family: Arial; font-size: 18px;" );
		m_loading->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
		m_loading->setFixedSize( 200, 100 );
	}
	m_loading->move( 200, 290 );
	m_loading->setText( str );
	m_loading->show();
	QCoreApplication::processEvents();
}

void JsExtender::closeLoading()
{
	if ( m_loading )
		m_loading->close();
}

void JsExtender::showSettings()
{
	SettingsDialog *s = new SettingsDialog( m_mainWindow );
	s->show();
}
