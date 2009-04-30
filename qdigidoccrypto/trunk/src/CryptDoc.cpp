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

#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocEncGen.h>
#include <libdigidoc/DigiDocEncSAXParser.h>
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
{
	initDigiDocLib();
	initConfigStore( NULL );
}

CryptDoc::~CryptDoc()
{
	cleanupConfigStore( NULL );
	finalizeDigiDocLib();
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
}

void CryptDoc::addKey( const CKey &key )
{
	if( isNull() )
		return setLastError( tr("Container is not open") );
	if( isEncrypted() )
		return setLastError( tr("Container is encrypted") );

	X509 *cert = (X509*)decodeCert( key.certPem );
	char *id = NULL;
	char *recipient = key.recipient.toUtf8().data();
	char *keyname = NULL;
	char *carriedkeyname = NULL;
	DEncEncryptedKey *pkey;
	int err = dencEncryptedKey_new( m_enc, &pkey, cert,
		DENC_ENC_METHOD_RSA1_5, id, recipient, keyname, carriedkeyname );
	if( err != ERR_OK )
		setLastError( tr("Failed to add key"), err );
}

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
}

void CryptDoc::create( const QString &file )
{
	clear();
	const char *format = ConfigItem_lookup("DIGIDOC_FORMAT");
	const char *version = "1.3"; //ConfigItem_lookup("DIGIDOC_VERSION");
	int err = SignedDoc_new( &m_doc, format, version );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to create document"), err );
		return;
	}

	err = dencEncryptedData_new( &m_enc, DENC_XMLNS_XMLENC, DENC_ENC_METHOD_AES128, 0, 0, 0 );
	if( err != ERR_OK )
	{
		setLastError( tr("Failed to create document"), err );
		clear();
		return;
	}
	m_fileName = file;
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

	DEncEncryptedKey *key;
	int err = dencEncryptedData_findEncryptedKeyByPKCS11( m_enc, &key );
	if( err != ERR_OK || !key )
	{
		setLastError( tr("Recipient does not exist in document recipient list"), err );
		return false;
	}

	err = dencEncryptedData_decrypt( m_enc, key, pin.toUtf8() );
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
	else
	{
		for( int i = 0; i < m_doc->nDataFiles; ++i )
		{
			DataFile *data = m_doc->pDataFiles[i];
			CDocument doc;
			doc.filename = QFileInfo( QString::fromUtf8( data->szFileName ) ).fileName();
			doc.mime = QString::fromUtf8( data->szMimeType );
			doc.size = QString::number( data->nSize );
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
		setLastError( tr("No recipiest keys") );
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
	char size[100];
	for( int i = 0; i < m_doc->nDataFiles; ++i )
	{
		DataFile *data = m_doc->pDataFiles[i];
		qsnprintf( id, 50, "orig_file%d", i );
		qsnprintf( size, 100, "%ld", data->nSize );
		int err = dencOrigContent_add( m_enc,
			id,
			QFileInfo( data->szFileName ).fileName().toUtf8(),
			size,
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
	return isEncrypted();
}

QString CryptDoc::fileName() const { return m_fileName; }

bool CryptDoc::isEncrypted() const
{
	return m_enc->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED ||
		m_enc->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED;
}

bool CryptDoc::isNull() const { return m_enc == 0; }

QList<CKey> CryptDoc::keys()
{
	QList<CKey> list;
	if( isNull() /*|| !isEncrypted()*/ )
		return list;

	for( int i = 0; i < m_enc->nEncryptedKeys; ++i )
	{
		CKey ckey;

		/*char *p = 0;
		int err = getCertPEM( m_enc->arrEncryptedKeys[i]->pCert, 1, &p );
		if( !err )
			ckey.certPem = p;
		if( p )
			free(p);*/

		ckey.id = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szId );
		ckey.name = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szKeyName );
		ckey.recipient = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szRecipient );
		ckey.type = QString::fromUtf8( m_enc->arrEncryptedKeys[i]->szEncryptionMethod );

		list << ckey;
	}

	return list;
}

QString CryptDoc::lastError() const { return m_lastError; }

void CryptDoc::open( const QString &file )
{
	clear();
	m_fileName = file;
	int err = dencSaxReadEncryptedData( &m_enc, file.toUtf8() );
	if( err != ERR_OK )
		setLastError( tr("Failed to open crypted document"), err );
}

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

void CryptDoc::setLastError( const QString &err, int errCode )
{ Q_EMIT error( m_lastError = err, errCode ); }
