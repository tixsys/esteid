#include <QtWebKit>
#include <QDesktopServices>
#include <QInputDialog>

#include "jsextender.h"

JsExtender::JsExtender(QWebFrame *frame)
:	QObject( frame )
{
    connectSignals(frame);
    m_webFrame = frame;

	jsSSL = new SSLConnect();
}

JsExtender::~JsExtender()
{
	if ( QFile::exists( m_tempFile ) )
		QFile::remove( m_tempFile );
}

void JsExtender::connectSignals(QWebFrame *frame)
{
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(javaScriptWindowObjectCleared()));
}

void JsExtender::registerObject(const QString &name, QObject *object)
{
    m_webFrame->addToJavaScriptWindowObject(name, object);

    m_registeredObjects[name] = object;
}

void JsExtender::javaScriptWindowObjectCleared()
{
    for (QMap<QString, QObject *>::Iterator it =
                               m_registeredObjects.begin();
                               it != m_registeredObjects.end(); ++it) {
        QString name = it.key();
        QObject *object = it.value();
        m_webFrame->addToJavaScriptWindowObject(name, object);
    }

    m_webFrame->addToJavaScriptWindowObject("extender", this);
}

QVariant JsExtender::jsCall(const QString &function, int argument)
{
    QString statement = QString( "%1(%2)" ).arg( function ).arg( argument );
    return m_webFrame->evaluateJavaScript(statement);
}

QVariant JsExtender::jsCall(const QString &function, const QString &argument)
{
    QString statement = QString( "%1(\"%2\")" ).arg( function ).arg( argument );
    return m_webFrame->evaluateJavaScript(statement);
}

QVariant JsExtender::jsCall(const QString &function, const QString &argument, const QString &argument2 )
{
    QString statement = QString( "%1(\"%2\",\"%3\")" ).arg( function ).arg( argument ).arg( argument2 );
    return m_webFrame->evaluateJavaScript(statement);
}

void JsExtender::openUrl( const QString &url )
{
	QDesktopServices::openUrl( QUrl( url ) );
}

QString JsExtender::checkPin()
{
	QString pin;
	if ( !jsSSL->isLoaded() )
	{
		bool ok;
		pin = QInputDialog::getText( 0, tr("Isikutuvastus"), tr("Sisesta PIN1"), QLineEdit::Password, QString(), &ok );
		if( !ok )
			pin = "";
	}
	return pin;
}

void JsExtender::activateEmail( const QString &email )
{
	std::vector<unsigned char> buffer = jsSSL->getUrl( "activateEmail", checkPin().toStdString(), email.toStdString() );
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
	std::vector<unsigned char> buffer = jsSSL->getUrl( "emails", checkPin().toStdString() );
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
	std::vector<unsigned char> buffer = jsSSL->getUrl( "picture", checkPin().toStdString() );
	if ( !buffer.size() )
	{
		jsCall( "setPicture", "" );
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
				jsCall( "setPicture", url );
				return;
			}
		}
	}

	jsCall( "setPicture", "" );
}
