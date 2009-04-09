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

#include <digidoc/BDoc.h>
#include <digidoc/XmlConf.h>
#include <digidoc/crypto/cert/DirectoryX509CertStore.h>
#include <digidoc/crypto/signer/EstEIDSigner.h>
#include <digidoc/io/ZipSerialize.h>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>

using namespace digidoc;

QSslCertificate createQSslCertificate( std::vector<unsigned char> data )
{ return QSslCertificate( QByteArray( (const char*)&data[0], data.size() ), QSsl::Der ); }

class QEstEIDSigner: public EstEIDSigner
{
public:
	QEstEIDSigner() throw(SignException);
	virtual ~QEstEIDSigner() {}

	QSslCertificate authCert() const { return m_authCert; }
	QSslCertificate signCert() const { return m_signCert; }

protected:
	virtual std::string getPin( PKCS11Cert certificate ) throw(SignException);
	virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(
		std::vector<PKCS11Signer::PKCS11Cert> certificates ) throw(SignException);

private:
	QSslCertificate m_authCert, m_signCert;
};


QEstEIDSigner::QEstEIDSigner() throw(SignException)
:	EstEIDSigner( Conf::getInstance()->getPKCS11DriverPath() )
{
	try	{ getCert(); }
	catch( const Exception & ) {}
}

std::string QEstEIDSigner::getPin( PKCS11Cert certificate ) throw(SignException)
{
	bool ok;
	QString pin = QInputDialog::getText( 0, "QDigiDocClient",
		QObject::tr("Selected action requires sign certificate.\n"
			"For using sign certificate enter PIN2"),
		QLineEdit::Password,
		QString(), &ok );
	if( !ok )
		throw SignException( __FILE__, __LINE__, "PIN acquisition canceled.");
	return pin.toStdString();
}

PKCS11Signer::PKCS11Cert QEstEIDSigner::selectSigningCertificate(
	std::vector<PKCS11Signer::PKCS11Cert> certificates ) throw(SignException)
{
	PKCS11Signer::PKCS11Cert signCert;
	Q_FOREACH( const PKCS11Signer::PKCS11Cert &cert, certificates )
	{
		try
		{
			if( cert.label == std::string("Isikutuvastus") )
				m_authCert = createQSslCertificate( X509Cert( cert.cert ).encodeDER() );
			else if( cert.label == std::string("Allkirjastamine") )
			{
				m_signCert = createQSslCertificate( X509Cert( cert.cert ).encodeDER() );
				signCert = cert;
			}
		}
		catch( const Exception & ) {}
	}
	if( signCert.label != std::string("Allkirjastamine") )
		SignException( __FILE__, __LINE__, "Could not find certificate with label 'Allkirjastamine'." );

	return signCert;
}



DigiDocSignature::DigiDocSignature( const digidoc::Signature *signature ): s(signature) {}

QSslCertificate DigiDocSignature::cert() const
{
	try { return createQSslCertificate( s->getSigningCertificate().encodeDER() ); }
	catch( const Exception & ) {}
	return QSslCertificate();
}

QDateTime DigiDocSignature::dateTime() const
{
	QDateTime t = QDateTime::fromString(
		s->getSigningTime().data(), "yyyy-MM-dd'T'hh:mm:ss'Z'" );
	return QDateTime( t.date(), t.time(), Qt::UTC ).toLocalTime();
}

bool DigiDocSignature::isValid() const
{
	try { return s->validateOnline() == OCSP::GOOD; }
	catch( const Exception & ) {}
	return false;
}

QString DigiDocSignature::location() const
{
	QStringList l;
	const Signer::SignatureProductionPlace p = s->getProductionPlace();
	if( !p.city.empty() )
		l << QString::fromUtf8( p.city.data() );
	if( !p.stateOrProvince.empty() )
		l << QString::fromUtf8( p.stateOrProvince.data() );
	if( !p.postalCode.empty() )
		l << QString::fromUtf8( p.postalCode.data() );
	if( !p.countryName.empty() )
		l << QString::fromUtf8( p.countryName.data() );

	return l.join( ", " );
}

QString DigiDocSignature::role() const
{
	QStringList list;
	const Signer::SignerRole::TRoles roles = s->getSignerRole().claimedRoles;
	Signer::SignerRole::TRoles::const_iterator i = roles.begin();
	for( ; i != roles.end(); ++i )
		list << QString::fromUtf8( i->data() );
	return list.join( ", " );
}



DigiDoc::DigiDoc( QObject *parent )
:	QObject( parent )
,	b(0)
{
	digidoc::initialize();
	Conf::init( new XmlConf( "/etc/libdigidoc/bdoclib.conf" ) );
	X509CertStore::init( new DirectoryX509CertStore() );

	try
	{
		m_signer = new QEstEIDSigner();
		m_authCert = m_signer->authCert();
		m_signCert = m_signer->signCert();
	}
	catch( const Exception &e ) { setLastError( e ); }
	delete m_signer;
	m_signer = 0;

	startTimer( 5 * 1000 );
}

DigiDoc::~DigiDoc()
{
	clear();
	X509CertStore::destroy();
	digidoc::terminate();
}

void DigiDoc::addFile( const QString &file )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( b->signatureCount() > 0 )
		return setLastError( tr("Cannot add files to signed container") );

	try { b->addDocument( Document( file.toStdString(), "file" ) ); }
	catch( const Exception &e ) { setLastError( e ); }
}

QSslCertificate DigiDoc::authCert() { return m_authCert; }
QSslCertificate DigiDoc::signCert() { return m_signCert; }

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
	b = new BDoc();
	m_fileName = file;
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

QString DigiDoc::fileName() const { return m_fileName; }
bool DigiDoc::isNull() const { return b == 0; }
QString DigiDoc::lastError() const { return m_lastError; }

void DigiDoc::open( const QString &file )
{
	clear();
	m_fileName = file;
	try
	{
		std::auto_ptr<ISerialize> s(new ZipSerialize(file.toStdString()));
		b = new BDoc( s );
	}
	catch( const Exception &e ) { setLastError( e ); }
}

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
		std::auto_ptr<ISerialize> s(new ZipSerialize(m_fileName.toStdString()));
		b->saveTo( s );
	}
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::saveDocument( unsigned int num, const QString &path )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	if( num >= b->documentCount() )
		return setLastError( tr("Missing document") );

	try
	{
		QFileInfo file( QString::fromStdString( b->getDocument( num ).getPath() ) );
		b->getDocument( num ).saveAs( QString( path + QDir::separator() + file.fileName() ).toStdString() );
	}
	catch( const Exception &e ) { setLastError( e ); }
}

void DigiDoc::setLastError( const Exception &e )
{
	QStringList causes;
	causes << QString::fromUtf8( e.getMsg().data() );
	Q_FOREACH( const Exception &c, e.getCauses() )
		causes << QString::fromUtf8( c.getMsg().data() );
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
	try
	{
		m_signer = new QEstEIDSigner();
		m_signer->setSignatureProductionPlace( Signer::SignatureProductionPlace(
			city.toUtf8().constData(),
			state.toUtf8().constData(),
			zip.toUtf8().constData(),
			country.toUtf8().constData() ) );
		Signer::SignerRole sRole( role.toUtf8().constData() );
		if ( !role2.isEmpty() )
			sRole.claimedRoles.push_back( role2.toUtf8().constData() );
		m_signer->setSignerRole( sRole );
		b->sign( m_signer, Signature::TM /*BES|TM*/ );
		result = true;
	}
	catch( const Exception &e ) { setLastError( e ); }
	delete m_signer;
	m_signer = 0;
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
			list << DigiDocSignature( b->getSignature( i ) );
	}
	catch( const Exception &e ) { setLastError( e ); }
	return list;
}

void DigiDoc::timerEvent( QTimerEvent * )
{
	if( m_signer != 0 )
		return;

	try
	{
		m_signer = new QEstEIDSigner();
		if( m_authCert != m_signer->authCert() || m_signCert != m_signer->signCert() )
		{
			m_authCert = m_signer->authCert();
			m_signCert = m_signer->signCert();
			Q_EMIT dataChanged();
		}
	}
	catch( const Exception &e ) { setLastError( e ); }
	delete m_signer;
	m_signer = 0;
}
