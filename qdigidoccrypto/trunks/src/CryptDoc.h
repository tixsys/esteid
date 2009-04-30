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

#pragma once

#include <QObject>

#include <QSslCertificate>

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocEnc.h>
#include <libdigidoc/DigiDocObj.h>

class CDocument
{
public:
	QString filename;
	QString mime;
	QString size;
};

class CKey
{
public:
	QByteArray certPem;
	QString id;
	QString name;
	QString recipient;
	QString type;
};

class CryptDoc: public QObject
{
	Q_OBJECT
public:
	CryptDoc( QObject *parent = 0 );
	~CryptDoc();

	void addFile( const QString &file, const QString &mime );
	void addKey( const CKey &key );
	void create( const QString &file );
	void clear();
	bool decrypt( const QString &pin );
	QList<CDocument> documents();
	bool encrypt();
	QString fileName() const;
	bool isEncrypted() const;
	bool isNull() const;
	QList<CKey> keys();
	QString lastError() const;
	void open( const QString &file );
	void removeDocument( int id );
	void removeKey( int id );
	void save();
	void saveDocument( int id, const QString &path );

Q_SIGNALS:
	void dataChanged();
	void error( const QString &err, int errCode );

private:
	void setLastError( const QString &err, int errCode = -1 );

	QString			m_ddocTemp;
	QString			m_fileName;
	QString			m_lastError;
	DEncEncryptedData *m_enc;
	SignedDoc		*m_doc;
};
