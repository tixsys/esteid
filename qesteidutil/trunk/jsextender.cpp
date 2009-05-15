#include <QtWebKit>
#include <QDesktopServices>
#include <QInputDialog>

#include "mainwindow.h"
#include "jsextender.h"

JsExtender::JsExtender( MainWindow *main )
:	QObject( main->page()->mainFrame() )
,	m_mainWindow( main )
{
    connectSignals();
	jsSSL = new SSLConnect();
}

JsExtender::~JsExtender()
{
	if ( QFile::exists( m_tempFile ) )
		QFile::remove( m_tempFile );
}

void JsExtender::connectSignals()
{
	connect( m_mainWindow->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(javaScriptWindowObjectCleared()));
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
	QString pin;
	if ( !jsSSL->isLoaded() )
	{
		bool ok;
		pin = QInputDialog::getText( m_mainWindow, tr("Isikutuvastus"), tr("Sisesta PIN1"), QLineEdit::Password, QString(), &ok );
		if( !ok )
			pin = "";
		else if ( !m_mainWindow->eidCard()->validatePin1( pin ) ) {
			jsCall( "handleError", "PIN1InvalidRetry" );
			pin = "";
		}
	}
	return pin;
}

void JsExtender::activateEmail( const QString &email )
{
	std::vector<unsigned char> buffer;
	try {
		buffer = jsSSL->getUrl( "activateEmail", checkPin().toStdString(), email.toStdString() );
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
		buffer = jsSSL->getUrl( "emails", checkPin().toStdString() );
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
		buffer = jsSSL->getUrl( "picture", checkPin().toStdString() );
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
