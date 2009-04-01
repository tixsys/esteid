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

#pragma once

#include <QObject>

#include <QSslCertificate>

namespace digidoc
{
	class BDoc;
	class Document;
	class Exception;
	class Signature;
}

class QDateTime;
class QEstEIDSigner;

class DigiDocSignature
{
public:
	DigiDocSignature( const digidoc::Signature *signature );

	QSslCertificate	cert() const;
	QDateTime dateTime() const;
	bool	isValid() const;
	QString	location() const;
	QString	role() const;

private:
	const digidoc::Signature *s;
};

class DigiDoc: public QObject
{
	Q_OBJECT
public:
	DigiDoc( QObject *parent = 0 );
	~DigiDoc();

	void addFile( const QString &file );
	QSslCertificate authCert();
	QSslCertificate signCert();
	void create( const QString &file );
	void clear();
	QList<digidoc::Document> documents();
	QString fileName() const;
	bool isNull() const;
	QString lastError() const;
	void open( const QString &file );
	void removeDocument( unsigned int num );
	void removeSignature( unsigned int num );
	void save();
	void saveDocuments( const QString &path );
	void sign(
		const QString &city,
		const QString &state,
		const QString &zip,
		const QString &country,
		const QString &role,
		const QString &role2 );
	QList<DigiDocSignature> signatures();

Q_SIGNALS:
	void dataChanged();
	void error( const QString &err );

private:
	void setLastError( const digidoc::Exception &e );
	void setLastError( const QString &err );
	void timerEvent( QTimerEvent *e );

	digidoc::BDoc	*b;
	QString			m_fileName;
	QString			m_lastError;
	QSslCertificate m_authCert, m_signCert;
	QEstEIDSigner	*m_signer;
};
