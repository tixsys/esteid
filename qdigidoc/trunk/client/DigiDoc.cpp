/*
 * QDigiDocClient
 *
 * Copyright (C) 2009 Jargo K�ster <jargo@innovaatik.ee>
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
#include "QSigner.h"

#include <digidocpp/Conf.h>
#include <digidocpp/DDoc.h>
#include <digidocpp/Document.h>
#include <digidocpp/SignatureTM.h>
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
		Signature *sc = const_cast<Signature*>(s);
		if( s->getMediaType().compare( "signature/bdoc-1.0/TM" ) == 0 )
		{
			SignatureTM *tm = static_cast<SignatureTM*>(sc);
			tm->getRevocationOCSPRef( data, method );
		}
		else if( s->getMediaType().compare( 0, 11, "DIGIDOC-XML" ) == 0 ||
			s->getMediaType().compare( 0, 6, "SK-XML" ) == 0 )
		{
			SignatureDDOC *ddoc = static_cast<SignatureDDOC*>(sc);
			ddoc->getRevocationOCSPRef( data, method );
		}
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
		Signature *sc = const_cast<Signature*>(s);
		if( s->getMediaType().compare( "signature/bdoc-1.0/TM" ) == 0 )
		{
			SignatureTM *tm = static_cast<SignatureTM*>(sc);
			tm->getRevocationOCSPRef( data, method );
		}
		else if( s->getMediaType().compare( 0, 11, "DIGIDOC-XML" ) == 0 ||
			s->getMediaType().compare( 0, 6, "SK-XML" ) == 0 )
		{
			SignatureDDOC *ddoc = static_cast<SignatureDDOC*>(sc);
			ddoc->getRevocationOCSPRef( data, method );
		}
		if( data.size() > 0 )
			return QByteArray( (const char*)&data[0], data.size() );
	}
	catch( const Exception & ) {}
	return QByteArray();
}

DigiDocSignature::SignatureStatus DigiDocSignature::validate()
{
	try
	{
		s->validateOffline();
		if( s->getMediaType() == "signature/bdoc-1.0/BES" )
		{
			switch( s->validateOnline() )
			{
			case OCSP::GOOD: return Valid;
			case OCSP::REVOKED: return Invalid;
			case OCSP::UNKNOWN: return Unknown;
			}
		}
		else
			return Valid;
	}
	catch( const Exception &e )
	{
		setLastError( e );
		if( e.code() == Exception::OCSPResponderMissing )
			return Unknown;
	}
	return Invalid;
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
,	signer(0)
{}

DigiDoc::~DigiDoc()
{
	delete signer;
	clear();
	X509CertStore::destroy();
	digidoc::terminate();
}

QString DigiDoc::activeCard() const { return m_card; }

void DigiDoc::addFile( const QString &file )
{
	if( !checkDoc( b->signatureCount() > 0, tr("Cannot add files to signed container") ) )
		return;
	try { b->addDocument( Document( file.toUtf8().constData(), "file" ) ); }
	catch( const Exception &e ) { setLastError( e ); }
}

bool DigiDoc::checkDoc( bool status, const QString &msg )
{
	if( isNull() )
		setLastError( tr("Container is not open") );
	else if( status )
		setLastError( msg );
	return !isNull() && !status;
}

void DigiDoc::clear()
{
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
	if( !checkDoc() )
		return list;
	try
	{
		unsigned int count = b->documentCount();
		for( unsigned int i = 0; i < count; ++i )
			list << b->getDocument( i );
	}
	catch( const Exception &e ) { setLastError( e ); }

	return list;
}

QString DigiDoc::getAccessCert()
{
	QString buffer;

	signer->unloadDriver();
	try {
		SSLConnect sslConnect;
		sslConnect.setPKCS11( getConfValue( PKCS11Module ) );
		sslConnect.setCard( m_card );
		buffer = QString::fromUtf8( sslConnect.getUrl( SSLConnect::AccessCert, "" ) );
	} catch( const std::runtime_error &e ) {
		if( !QByteArray( e.what() ).isEmpty() )
			setLastError( e.what() );
	}

	signer->start();

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

	signer = new QSigner();
	connect( signer, SIGNAL(dataChanged(QStringList,QString,QSslCertificate)),
		SLOT(dataChanged(QStringList,QString,QSslCertificate)) );
	connect( signer, SIGNAL(error(QString)), SLOT(setLastError(QString)) );
	signer->start();
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
	catch( const Exception &e )
	{
		QStringList causes;
		parseException( e, causes );
		setLastError( tr("An error occurred while opening the document.<br />%1").arg( causes.join("\n") ) );
	}
	return false;
}

bool DigiDoc::parseException( const Exception &e, QStringList &causes )
{
	switch( e.code() )
	{
	case Exception::CertificateRevoked:
		setLastError( tr("Certificate status revoked") ); return false;
	case Exception::CertificateUnknown:
		setLastError( tr("Certificate status unknown") ); return false;
	case Exception::OCSPTimeSlot:
		setLastError( tr("Check your computer time") ); return false;
	case Exception::PINCanceled:
		return false;
	case Exception::PINFailed:
		setLastError( tr("PIN Login failed") ); return false;
	case Exception::PINIncorrect:
		setLastError( tr("PIN Incorrect") ); return false;
	case Exception::PINLocked:
		setLastError( tr("PIN Locked") ); return false;
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
	if( !checkDoc( num >= b->documentCount(), tr("Missing document") ) )
		return;
	try { b->removeDocument( num ); }
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::removeSignature( unsigned int num )
{
	if( !checkDoc( num >= b->signatureCount(), tr("Missing signature") ) )
		return;
	try { b->removeSignature( num ); }
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::save()
{
	/*if( !checkDoc() );
		return; */
	try
	{
		std::auto_ptr<ISerialize> s(new ZipSerialize(m_fileName.toUtf8().constData()));
		b->saveTo( s );
	}
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::saveDocument( unsigned int num, const QString &filepath )
{
	if( !checkDoc( num >= b->documentCount(), tr("Missing document") ) )
		return;
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
{ QMetaObject::invokeMethod( signer, "selectCard", Qt::QueuedConnection, Q_ARG(QString,card) ); }

QString DigiDoc::getConfValue( ConfParameter parameter, const QVariant &value ) const
{
	digidoc::Conf *i = NULL;
	try { i = digidoc::Conf::getInstance(); }
	catch( const Exception & ) { return value.toString(); }

	std::string r;
	switch( parameter )
	{
	case PKCS11Module: r = i->getPKCS11DriverPath(); break;
	case ProxyHost: r = i->getProxyHost(); break;
	case ProxyPort: r = i->getProxyPort(); break;
	case ProxyUser: r = i->getProxyUser(); break;
	case ProxyPass: r = i->getProxyPass(); break;
	case PKCS12Cert: r = i->getPKCS12Cert(); break;
	case PKCS12Pass: r = i->getPKCS12Pass(); break;
	default: break;
	}
	return r.empty() ? value.toString() : QString::fromStdString( r );
}

void DigiDoc::setConfValue( ConfParameter parameter, const QVariant &value )
{
	digidoc::Conf *i = NULL;
	try { i = digidoc::Conf::getInstance(); }
	catch( const Exception & ) { return; }

	const std::string v = value.toString().toStdString();
	switch( parameter )
	{
	case ProxyHost: i->setProxyHost( v ); break;
	case ProxyPort: i->setProxyPort( v ); break;
	case ProxyUser: i->setProxyUser( v ); break;
	case ProxyPass: i->setProxyPass( v ); break;
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
	if( !checkDoc( b->documentCount() == 0, tr("Cannot add signature to empty container") ) )
		return false;

	bool result = false;
	try
	{
		signer->setSignatureProductionPlace( Signer::SignatureProductionPlace(
			city.toUtf8().constData(),
			state.toUtf8().constData(),
			zip.toUtf8().constData(),
			country.toUtf8().constData() ) );
		Signer::SignerRole sRole( role.toUtf8().constData() );
		if ( !role2.isEmpty() )
			sRole.claimedRoles.push_back( role2.toUtf8().constData() );
		signer->setSignerRole( sRole );
		b->sign( signer, Signature::TM );
		result = true;
	}
	catch( const Exception &e ) { setLastError( e ); }
	return result;
}

QSslCertificate DigiDoc::signCert() { return m_signCert; }

bool DigiDoc::signMobile( const QString &fName )
{
	if( !checkDoc( b->documentCount() == 0, tr("Cannot add signature to empty container") ) )
		return false;

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
	if( !checkDoc() )
		return list;
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
{ return checkDoc() ? b->documentType() : WDoc::BDocType; }

QByteArray DigiDoc::getFileDigest( unsigned int i )
{
	QByteArray result;
	if( !checkDoc() )
		return result;
	result.resize(20);
	b->getFileDigest( i, (unsigned char*)result.data() );
	return result;
}
