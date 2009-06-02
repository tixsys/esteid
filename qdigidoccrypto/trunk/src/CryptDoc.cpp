/*
 * QDigiDocCrypto
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

#include "CryptDoc.h"

#include "Common.h"
#include "Poller.h"
#include "SslCertificate.h"

#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocEncGen.h>
#include <libdigidoc/DigiDocEncSAXParser.h>
#include <libdigidoc/DigiDocPKCS11.h>
#include <libdigidoc/DigiDocSAXParser.h>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>
#include <QTemporaryFile>

CryptDoc::CryptDoc( QObject *parent )
:	QObject( parent )
,	m_enc(0)
,	m_doc(0)
,	modified(false)
{
	initDigiDocLib();
	initConfigStore( NULL );

	poller = new Poller();
	connect( poller, SIGNAL(dataChanged(QStringList,QString,QSslCertificate,QSslCertificate)),
		SLOT(dataChanged(QStringList,QString,QSslCertificate,QSslCertificate)) );
	poller->start();
}

CryptDoc::~CryptDoc()
{
	delete poller;
	cleanupConfigStore( NULL );
	finalizeDigiDocLib();
}

QString CryptDoc::activeCard() const { return m_card; }

void CryptDoc::addCardCert()
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	poller->lock();
	X509 *cert;
	int err = findUsersCertificate( 0, &cert );
	if( err != ERR_OK )
	{
		poller->unlock();
		return setLastError( tr("Din't find card certificate") );
	}
	poller->unlock();

	SslCertificate c = SslCertificate::fromX509( (Qt::HANDLE*)cert );
	free( cert );

	CKey key;
	key.cert = c;
	key.recipient = c.subjectInfoUtf8( "CN" );
	addKey( key );
}

void CryptDoc::addFile( const QString &file, const QString &mime )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	DataFile *data;
	int err = DataFile_new( &data, m_doc, NULL, file.toUtf8(),
		CONTENT_EMBEDDED_BASE64, mime.toUtf8(), 0, NULL, 0, NULL, CHARSET_UTF_8 );
	if( err != ERR_OK )
		return setLastError( tr("Failed to add file"), err );

	err = calculateDataFileSizeAndDigest( m_doc, data->szId, file.toUtf8(), DIGEST_SHA1 );
	if( err != ERR_OK )
		setLastError( tr("Failed to calculate digest"), err );
	modified = true;
}

void CryptDoc::addKey( const CKey &key )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	DEncEncryptedKey *pkey;
	int err = dencEncryptedKey_new( m_enc, &pkey, (X509*)decodeCert( key.cert.toPem() ),
		DENC_ENC_METHOD_RSA1_5, NULL, key.recipient.toUtf8(), NULL, NULL );
	if( err != ERR_OK )
		setLastError( tr("Failed to add key"), err );
	else
		modified = true;
}

QSslCertificate CryptDoc::authCert() const { return m_authCert; }

void CryptDoc::clear()
{
	if( m_enc != 0 )
		dencEncryptedData_free( m_enc );
	if( m_doc != 0 )
		SignedDoc_free( m_doc );
	m_enc = 0;
	m_doc = 0;
	m_fileName.clear();
	m_lastError.clear();
	if( !m_ddocTemp.isEmpty() )
	{
		QFile::remove( m_ddocTemp );
		m_ddocTemp.clear();
	}
	modified = false;
}

void CryptDoc::create( const QString &file )
{
	clear();
	const char *format = "DIGIDOC-XML"; //ConfigItem_lookup("DIGIDOC_FORMAT");
	const char *version = "1.3"; //ConfigItem_lookup("DIGIDOC_VERSION");

	int err = SignedDoc_new( &m_doc, format, version );
	if( err != ERR_OK )
		return setLastError( tr("Failed to create document"), err );

	err = dencEncryptedData_new( &m_enc, DENC_XMLNS_XMLENC, DENC_ENC_METHOD_AES128, 0, 0, 0 );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to create document"), err );
		clear();
		return;
	}
	m_fileName = file;
}

void CryptDoc::dataChanged( const QStringList &cards, const QString &card,
	const QSslCertificate &auth, const QSslCertificate &sign )
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
	if( m_authCert != auth || m_signCert != sign )
	{
		changed = true;
		m_authCert = auth;
		m_signCert = sign;
	}
	if( changed )
		Q_EMIT dataChanged();
}

bool CryptDoc::decrypt( const QString &pin )
{
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return false;
	}
	if( !isEncrypted() )
		return true;

	poller->lock();
	DEncEncryptedKey *key;
	int err = dencEncryptedData_findEncryptedKeyByPKCS11( m_enc, &key );
	if( err != ERR_OK || !key )
	{
		setLastError( tr("Recipient does not exist in document recipient list"), err );
		poller->unlock();
		return false;
	}

	err = dencEncryptedData_decrypt( m_enc, key, pin.toUtf8() );
	poller->unlock();
	if( err != ERR_OK )
	{
		setLastError( tr("Failed decrypt data"), err );
		return false;
	}

	QTemporaryFile f( QString( "%1%2XXXXXX.ddoc" )
		.arg( QDir::tempPath() ).arg( QDir::separator() ) );
	f.setAutoRemove( false );
	if( f.open() )
	{
		m_ddocTemp = f.fileName();
		f.write( (const char*)m_enc->mbufEncryptedData.pMem, m_enc->mbufEncryptedData.nLen );
		f.close();
		err = ddocSaxReadSignedDocFromFile( &m_doc, m_ddocTemp.toUtf8(), 0, 300 );
	}
	else
	{
		err = ddocSaxReadSignedDocFromMemory( &m_doc,
			m_enc->mbufEncryptedData.pMem, m_enc->mbufEncryptedData.nLen, 300 );
	}

	if( err != ERR_OK )
	{
		setLastError( tr("Failed to read decrypted data"), err );
		return false;
	}

	for( int i = m_enc->encProperties.nEncryptionProperties - 1; i >= 0; --i )
	{
		DEncEncryptionProperty *p = m_enc->encProperties.arrEncryptionProperties[i];
		if( qstrncmp( p->szName, "orig_file", 9 ) == 0 )
			dencEncryptedData_DeleteEncryptionProperty( m_enc, i );
	}
	modified = true;
	return !isEncrypted();
}

QList<CDocument> CryptDoc::documents()
{
	QList<CDocument> list;
	if( isNull() )
		return list;

	if( isEncrypted() )
	{
		int count = dencOrigContent_count( m_enc );
		for( int i = 0; i < count; ++i )
		{
			char filename[255], size[255], mime[255], id[255];
			dencOrigContent_findByIndex( m_enc, i, filename, size, mime, id );
			CDocument doc;
			doc.filename = QString::fromUtf8( filename );
			doc.mime = QString::fromUtf8( mime );
			doc.size = QString::fromUtf8( size );
			list << doc;
		}
	}
	else if( m_doc )
	{
		for( int i = 0; i < m_doc->nDataFiles; ++i )
		{
			DataFile *data = m_doc->pDataFiles[i];
			CDocument doc;
			doc.filename = QFileInfo( QString::fromUtf8( data->szFileName ) ).fileName();
			doc.mime = QString::fromUtf8( data->szMimeType );
			doc.size = Common::fileSize( data->nSize );
			list << doc;
		}
	}
	return list;
}

bool CryptDoc::encrypt()
{
	if( isNull() )
	{
		setLastError( tr("Container is not open") );
		return false;
	}
	if( isEncrypted() )
		return true;
	if( m_enc->nEncryptedKeys < 1 )
	{
		setLastError( tr("No recipient keys") );
		return false;
	}

	int err = ERR_OK;

#if 0
	err = dencOrigContent_registerDigiDoc( m_enc, m_doc );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to encrypt data"), err );
		return false;
	}
#else // To avoid full file path
	char id[50];
	for( int i = 0; i < m_doc->nDataFiles; ++i )
	{
		DataFile *data = m_doc->pDataFiles[i];
		qsnprintf( id, 50, "orig_file%d", i );
		int err = dencOrigContent_add( m_enc,
			id,
			QFileInfo( data->szFileName ).fileName().toUtf8(),
			Common::fileSize( data->nSize ).toUtf8(),
			data->szMimeType,
			data->szId );
		if( err != ERR_OK )
		{
			setLastError( tr("Failed to encrypt data"), err );
			return false;
		}
	}
#endif

	QFile f( QString( m_fileName ).append( ".ddoc" ) );
	err = createSignedDoc( m_doc, NULL, f.fileName().toUtf8() );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to encrypt data"), err );
		return false;
	}

	if( !f.open( QIODevice::ReadOnly ) )
	{
		setLastError( tr("Failed to encrypt data") );
		return false;
	}

	err = dencEncryptedData_AppendData( m_enc, f.readAll(), f.size() );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to encrypt data"), err );
		return false;
	}
	f.close();
	f.remove();

	err = dencEncryptedData_encryptData( m_enc, DENC_COMPRESS_NEVER );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to encrypt data"), err );
		return false;
	}

	SignedDoc_free( m_doc );
	m_doc = 0;
	modified = true;
	return isEncrypted();
}

QString CryptDoc::fileName() const { return m_fileName; }

bool CryptDoc::isEncrypted() const
{
	return m_enc &&
		(m_enc->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED ||
		m_enc->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED);
}

bool CryptDoc::isModified() const { return modified; }
bool CryptDoc::isNull() const { return m_enc == 0; }
bool CryptDoc::isSigned() const { return m_doc && m_doc->nSignatures; }

QList<CKey> CryptDoc::keys()
{
	QList<CKey> list;
	if( isNull() )
		return list;

	for( int i = 0; i < m_enc->nEncryptedKeys; ++i )
	{
		CKey ckey;
		ckey.cert = SslCertificate::fromX509( (Qt::HANDLE*)m_enc->arrEncryptedKeys[i]->pCert );
		ckey.id = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szId );
		ckey.name = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szKeyName );
		ckey.recipient = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szRecipient );
		ckey.type = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szEncryptionMethod );

		list << ckey;
	}

	return list;
}

QString CryptDoc::lastError() const { return m_lastError; }

bool CryptDoc::open( const QString &file )
{
	clear();
	m_fileName = file;
	int err = dencSaxReadEncryptedData( &m_enc, file.toUtf8() );
	if( err != ERR_OK )
		setLastError( tr("Failed to open crypted document"), err );
	return err == ERR_OK;
}

QStringList CryptDoc::presentCards() const { return m_cards; }

void CryptDoc::removeDocument( int id )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	if( !m_doc || id >= m_doc->nDataFiles || !m_doc->pDataFiles[id] )
		return setLastError( tr("Missing document") );

	int err = DataFile_delete( m_doc, m_doc->pDataFiles[id]->szId );
	if( err != ERR_OK )
		setLastError( tr("Failed to remove file"), err );
	else
		modified = true;
}

void CryptDoc::removeKey( int id )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );

	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	if( !m_enc || id >= m_enc->nEncryptedKeys || !m_enc->arrEncryptedKeys[id] )
		return setLastError( tr("Missing key") );

	int err = dencEncryptedData_DeleteEncryptedKey( m_enc, id );
	if( err != ERR_OK )
		setLastError( tr("Failed to remove key"), err );
	else
		modified = true;
}

void CryptDoc::save()
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( !isEncrypted() )
		return setLastError( tr("Container is not crypted") );

	int err = dencGenEncryptedData_writeToFile( m_enc, m_fileName.toUtf8() );
	if( err != ERR_OK )
		setLastError( tr("Failed to save encrpyted file"), err );
	else
		modified = false;
}

bool CryptDoc::saveDDoc( const QString &filename )
{
	if( !m_doc )
	{
		setLastError( tr("Document not open") );
		return false;
	}

	int err = createSignedDoc( m_doc, NULL, filename.toUtf8() );
	if( err != ERR_OK )
		setLastError( tr("Failed to save file"), err );
	return err == ERR_OK;
}

void CryptDoc::saveDocument( int id, const QString &path )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	if( !m_doc || id >= m_doc->nDataFiles || !m_doc->pDataFiles[id] )
		return setLastError( tr("Missing document") );

	QFileInfo source( QString::fromUtf8( m_doc->pDataFiles[id]->szFileName ) );
	QString destination = QString( "%1%2%3" )
		.arg( path ).arg( QDir::separator() ).arg( source.fileName() );
	if( source.isAbsolute() )
	{
		QFile::remove( destination );
		bool err = QFile::copy( source.absoluteFilePath(), destination );
		if( !err )
			return setLastError( tr("Failed to save file"), err );
	}
	else
	{
		int err = ddocSaxExtractDataFile( m_doc, m_ddocTemp.toUtf8(),
			destination.toUtf8(), m_doc->pDataFiles[id]->szId, CHARSET_UTF_8 );
		if( err != ERR_OK )
			return setLastError( tr("Failed to save file"), err );
	}
}

void CryptDoc::selectCard( const QString &card )
{ poller->selectCard( card ); }

void CryptDoc::setLastError( const QString &err, int code )
{
	QString errMsg;
	if( err > 0 ) errMsg = getErrorString( code );
	Q_EMIT error( m_lastError = err, code, errMsg );
}

QSslCertificate CryptDoc::signCert() const { return m_signCert; }
