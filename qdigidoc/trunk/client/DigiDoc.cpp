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

#include "DigiDoc.h"

#include "common/SslCertificate.h"
#include "common/sslConnect.h"

#include "QMobileSigner.h"
#include "Poller.h"

#include <digidocpp/BDoc.h>
#include <digidocpp/XmlConf.h>
#include <digidocpp/WDoc.h>
#include <digidocpp/crypto/cert/DirectoryX509CertStore.h>
#include <digidocpp/io/ZipSerialize.h>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include <stdexcept>

#ifndef BDOCLIB_CONF_PATH
#define BDOCLIB_CONF_PATH "digidocpp.conf"
#endif

using namespace digidoc;

DigiDocSignature::DigiDocSignature( const digidoc::Signature *signature, DigiDoc *parent )
:	s(signature)
,	m_parent(parent)
{}

QSslCertificate DigiDocSignature::cert() const
{
	QSslCertificate c;
	try
	{
		X509 *x509 = s->getSigningCertificate().getX509();
		c = SslCertificate::fromX509( (Qt::HANDLE)x509 );
		X509_free( x509 );
	}
	catch( const Exception & ) {}
	return c;
}

QDateTime DigiDocSignature::dateTime() const
{
	return SslCertificate::toLocalTime( QDateTime::fromString(
		s->getSigningTime().data(), "yyyy-MM-dd'T'hh:mm:ss'Z'" ) );
}

QString DigiDocSignature::digestMethod() const
{
	try
	{
		std::vector<unsigned char> data;
		std::string method;
		s->getRevocationOCSPRef( data, method );
		return QString::fromStdString( method );
	}
	catch( const Exception & ) {}
	return QString();
}

QByteArray DigiDocSignature::digestValue() const
{
	try
	{
		std::vector<unsigned char> data;
		std::string method;
		s->getRevocationOCSPRef( data, method );
		if( data.size() > 0 )
			return QByteArray( (const char*)&data[0], data.size() );
	}
	catch( const Exception & ) {}
	return QByteArray();
}

bool DigiDocSignature::isValid()
{
	try
	{
		s->validateOffline();
		if( s->getMediaType() == "signature/bdoc-1.0/BES" )
			return s->validateOnline() == OCSP::GOOD;
		else
			return true;
	}
	catch( const Exception &e ) { setLastError( e ); }
	return false;
}

QString DigiDocSignature::lastError() const { return m_lastError; }

QString DigiDocSignature::location() const
{
	QStringList l = locations();
	l.removeAll( "" );
	return l.join( ", " );
}

QStringList DigiDocSignature::locations() const
{
	QStringList l;
	const Signer::SignatureProductionPlace p = s->getProductionPlace();
	l << QString::fromUtf8( p.city.data() ).trimmed();
	l << QString::fromUtf8( p.stateOrProvince.data() ).trimmed();
	l << QString::fromUtf8( p.postalCode.data() ).trimmed();
	l << QString::fromUtf8( p.countryName.data() ).trimmed();
	return l;
}

QString DigiDocSignature::mediaType() const
{ return QString::fromUtf8( s->getMediaType().data() ); }

DigiDoc* DigiDocSignature::parent() const { return m_parent; }

QString DigiDocSignature::role() const
{
	QStringList r = roles();
	r.removeAll( "" );
	return r.join( ", " );
}

QStringList DigiDocSignature::roles() const
{
	QStringList list;
	const Signer::SignerRole::TRoles roles = s->getSignerRole().claimedRoles;
	Signer::SignerRole::TRoles::const_iterator i = roles.begin();
	for( ; i != roles.end(); ++i )
		list << QString::fromUtf8( i->data() ).trimmed();
	return list;
}

void DigiDocSignature::setLastError( const Exception &e )
{
	QStringList causes;
	causes << QString::fromUtf8( e.getMsg().data() );
	Q_FOREACH( const Exception &c, e.getCauses() )
		causes << QString::fromUtf8( c.getMsg().data() );
	m_lastError = causes.join( "\n" );
}



DigiDoc::DigiDoc( QObject *parent )
:	QObject( parent )
,	b(0)
,	poller(0)
{}

DigiDoc::~DigiDoc()
{
	if( poller )
		delete poller;
	clear();
	X509CertStore::destroy();
	digidoc::terminate();
}

QString DigiDoc::activeCard() const { return m_card; }

void DigiDoc::addFile( const QString &file )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( b->signatureCount() > 0 )
		return setLastError( tr("Cannot add files to signed container") );

	try { b->addDocument( Document( file.toUtf8().constData(), "file" ) ); }
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::clear()
{
	if( b != 0 )
		delete b;
	b = 0;
	m_fileName.clear();
	m_lastError.clear();
}

void DigiDoc::create( const QString &file )
{
	clear();
	QString type = QFileInfo( file ).suffix().toLower();
	if( type == "bdoc" )
	{
		b = new WDoc( WDoc::BDocType );
		m_fileName = file;
	}
	else if( type == "ddoc" )
	{
		b = new WDoc( WDoc::DDocType );
		m_fileName = file;
	}
}

void DigiDoc::dataChanged( const QStringList &cards, const QString &card,
	const QSslCertificate &sign )
{
	bool changed = false;
	if( m_cards != cards )
	{
		changed = true;
		m_cards = cards;
	}
	if( m_card != card )
	{
		changed = true;
		m_card = card;
	}
	if( m_signCert != sign )
	{
		changed = true;
		m_signCert = sign;
	}
	if( changed )
		Q_EMIT dataChanged();
}

QList<Document> DigiDoc::documents()
{
	QList<Document> list;
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return list;
	}

	try
	{
		unsigned int count = b->documentCount();
		for( unsigned int i = 0; i < count; ++i )
			list << b->getDocument( i );
	}
	catch( const Exception &e ) { setLastError( e ); }

	return list;
}

QByteArray DigiDoc::getAccessCert()
{
	QByteArray buffer;

	poller->stop();
	try {
		SSLConnect sslConnect;
		sslConnect.setCard( m_card );
		buffer = sslConnect.getUrl( SSLConnect::AccessCert, "" );
	} catch( const std::runtime_error &e ) {
		if( !QByteArray( e.what() ).isEmpty() )
			setLastError( e.what() );
	}

	poller->start();

	return buffer;
}

QString DigiDoc::fileName() const { return m_fileName; }

bool DigiDoc::init()
{
	if( qgetenv( "BDOCLIB_CONF_XML" ).isEmpty() )
	{
		QByteArray path = QSettings( QSettings::NativeFormat, QSettings::SystemScope,
			"Estonian ID Card", "digidocpp" ).value( "ConfigFile" ).toByteArray();
		if( !path.isEmpty() )
			qputenv( "BDOCLIB_CONF_XML", path );
		else
			qputenv( "BDOCLIB_CONF_XML", BDOCLIB_CONF_PATH );
	}

	try
	{
		digidoc::initialize();
		X509CertStore::init( new DirectoryX509CertStore() );
	}
	catch( const Exception &e ) { setLastError( e ); return false; }

	poller = new Poller();
	connect( poller, SIGNAL(dataChanged(QStringList,QString,QSslCertificate)),
		SLOT(dataChanged(QStringList,QString,QSslCertificate)) );
	connect( poller, SIGNAL(error(QString)), SLOT(setLastError(QString)) );
	poller->start();
	return true;
}

bool DigiDoc::isNull() const { return b == 0; }
QString DigiDoc::lastError() const { return m_lastError; }

bool DigiDoc::open( const QString &file )
{
	clear();
	m_fileName = file;
	try
	{
		std::auto_ptr<ISerialize> s(new ZipSerialize(file.toUtf8().constData()));
		b = new WDoc( s );
		return true;
	}
	catch( const Exception &e ) { setLastError( e ); }
	return false;
}

bool DigiDoc::parseException( const Exception &e, QStringList &causes )
{
	switch( e.code() )
	{
	case Exception::CertificateRevoked:
		setLastError( tr("Certificate status revoked") );
		return false;
	case Exception::CertificateUnknown:
		setLastError( tr("Certificate status unknown") );
		return false;
	case Exception::PINCanceled:
		return false;
	case Exception::PINFailed:
		setLastError( tr("PIN Login failed") );
		return false;
	case Exception::PINIncorrect:
		setLastError( tr("PIN Incorrect") );
		return false;
	case Exception::PINLocked:
		setLastError( tr("PIN Locked") );
		return false;
	default:
		causes << QString::fromUtf8( e.getMsg().data() );
		break;
	}
	Q_FOREACH( const Exception &c, e.getCauses() )
		if( !parseException( c, causes ) )
			return false;
	return true;
}

QStringList DigiDoc::presentCards() const { return m_cards; }

void DigiDoc::removeDocument( unsigned int num )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	if( num >= b->documentCount() )
		return setLastError( tr("Missing document") );

	try { b->removeDocument( num ); }
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::removeSignature( unsigned int num )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	if( num >= b->signatureCount() )
		return setLastError( tr("Missing signature") );

	try { b->removeSignature( num ); }
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::save()
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	try
	{
		std::auto_ptr<ISerialize> s(new ZipSerialize(m_fileName.toUtf8().constData()));
		b->saveTo( s );
	}
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::saveDocument( unsigned int num, const QString &filepath )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	if( num >= b->documentCount() )
		return setLastError( tr("Missing document") );

	try
	{
		digidoc::Document doc = b->getDocument( num );
		if( doc.getPath().data() == filepath.toUtf8() )
			return;
		QFile::remove( filepath );
		doc.saveAs( filepath.toUtf8().constData() );
	}
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::selectCard( const QString &card )
{ QMetaObject::invokeMethod( poller, "selectCard", Qt::QueuedConnection, Q_ARG(QString,card) ); }

QString DigiDoc::getConfValue( ConfParameter parameter, const QVariant &value ) const
{
	const QString v = value.toString();
	digidoc::Conf *i = NULL;
	try { i = digidoc::XmlConf::getInstance(); }
	catch( const Exception & ) {}
	if( !i )
		return v;

	std::string r;
	switch( parameter )
	{
	case ProxyHost: r = i->getProxyHost(); break;
	case ProxyPort: r = i->getProxyPort(); break;
	case PKCS12Cert: r = i->getPKCS12Cert(); break;
	case PKCS12Pass: r = i->getPKCS12Pass(); break;
	default: break;
	}
	return r.empty() ? v : QString::fromStdString( r );
}

void DigiDoc::setConfValue( ConfParameter parameter, const QVariant &value )
{
	digidoc::Conf *i = NULL;
	try { i = digidoc::XmlConf::getInstance(); }
	catch( const Exception & ) {}
	if( !i )
		return;

	const std::string v = value.toString().toStdString();
	switch( parameter )
	{
	case ProxyHost: i->setProxyPort( v ); break;
	case ProxyPort: i->setProxyHost( v ); break;
	case PKCS12Cert: i->setPKCS12Cert( v ); break;
	case PKCS12Pass: i->setPKCS12Pass( v ); break;
	default: break;
	}
}

void DigiDoc::setLastError( const Exception &e )
{
	QStringList causes;
	if( parseException( e, causes ) )
		setLastError( causes.join( "\n" ) );
}

void DigiDoc::setLastError( const QString &err ) { Q_EMIT error( m_lastError = err ); }

bool DigiDoc::sign( const QString &city, const QString &state, const QString &zip,
	const QString &country, const QString &role, const QString &role2 )
{
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return false;
	}
	if( b->documentCount() == 0 )
	{
		setLastError( tr("Cannot add signature to empty container") );
		return false;
	}

	bool result = false;
	QEstEIDSigner *s;
	try
	{
		poller->stop();
		s = new QEstEIDSigner( m_card );
		s->setSignatureProductionPlace( Signer::SignatureProductionPlace(
			city.toUtf8().constData(),
			state.toUtf8().constData(),
			zip.toUtf8().constData(),
			country.toUtf8().constData() ) );
		Signer::SignerRole sRole( role.toUtf8().constData() );
		if ( !role2.isEmpty() )
			sRole.claimedRoles.push_back( role2.toUtf8().constData() );
		s->setSignerRole( sRole );
		b->sign( s, Signature::TM );
		result = true;
	}
	catch( const Exception &e ) { setLastError( e ); }
	delete s;
	poller->start();
	return result;
}

QSslCertificate DigiDoc::signCert() { return m_signCert; }

bool DigiDoc::signMobile( const QString &fName )
{
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return false;
	}
	if( b->documentCount() == 0 )
	{
		setLastError( tr("Cannot add signature to empty container") );
		return false;
	}

	bool result = false;
	try
	{
		b->sign( new digidoc::QMobileSigner( fName ), Signature::MOBILE );
		result = true;
	}
	catch( const Exception &e ) { setLastError( e ); }
	return result;
}

QList<DigiDocSignature> DigiDoc::signatures()
{
	QList<DigiDocSignature> list;
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return list;
	}

	try
	{
		unsigned int count = b->signatureCount();
		for( unsigned int i = 0; i < count; ++i )
			list << DigiDocSignature( b->getSignature( i ), this );
	}
	catch( const Exception &e ) { setLastError( e ); }
	return list;
}

WDoc::DocumentType DigiDoc::documentType()
{
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return WDoc::BDocType;
	}

	return b->documentType();
}

QByteArray DigiDoc::getFileDigest( unsigned int i )
{
	QByteArray result;
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return result;
	}

	result.resize(20);
	b->getFileDigest( i, (unsigned char*)result.data() );

	return result;
}
