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
#include <QStringList>

#include <digidocpp/WDoc.h>

namespace digidoc
{
	class Document;
	class Exception;
	class Signer;
	class Signature;
}

class DigiDoc;
class QDateTime;
class Poller;

class DigiDocSignature
{
public:
	DigiDocSignature( const digidoc::Signature *signature, DigiDoc *parent );

	QSslCertificate	cert() const;
	QDateTime	dateTime() const;
	QString		digestMethod() const;
	QByteArray	digestValue() const;
	bool		isValid();
	QString		lastError() const;
	QString		location() const;
	QStringList locations() const;
	QString		mediaType() const;
	DigiDoc		*parent() const;
	QString		role() const;
	QStringList	roles() const;

private:
	void setLastError( const digidoc::Exception &e );

	const digidoc::Signature *s;
	QString m_lastError;
	DigiDoc *m_parent;
};

class DigiDoc: public QObject
{
	Q_OBJECT
public:
	enum ConfParameter
	{
		ProxyHost,
		ProxyPort,
		PKCS12Cert,
		PKCS12Pass,
	};

	DigiDoc( QObject *parent = 0 );
	~DigiDoc();

	QString activeCard() const;
	void addFile( const QString &file );
	void create( const QString &file );
	void clear();
	QList<digidoc::Document> documents();
	QByteArray getAccessCert();
	QString fileName() const;
	void init();
	bool isNull() const;
	QString lastError() const;
	bool open( const QString &file );
	QStringList presentCards() const;
	void removeDocument( unsigned int num );
	void removeSignature( unsigned int num );
	void save();
	void saveDocument( unsigned int num, const QString &filepath );
	QString getConfValue( ConfParameter parameter, const QVariant &value ) const;
	static void setConfValue( ConfParameter parameter, const QVariant &value );
	bool sign(
		const QString &city,
		const QString &state,
		const QString &zip,
		const QString &country,
		const QString &role,
		const QString &role2 );
	QSslCertificate signCert();
	bool signMobile( const QString &fName );
	QList<DigiDocSignature> signatures();
	digidoc::WDoc::DocumentType documentType();

Q_SIGNALS:
	void dataChanged();
	void error( const QString &err );

private Q_SLOTS:
	void dataChanged( const QStringList &cards, const QString &card,
		const QSslCertificate &sign );
	void selectCard( const QString &card );
	void setLastError( const QString &err );

private:
	void parseException( const digidoc::Exception &e, QStringList &causes );
	void setLastError( const digidoc::Exception &e );

	digidoc::WDoc	*b;
	Poller			*poller;
	QSslCertificate	m_signCert;
	QStringList		m_cards;
	QString			m_card;
	QString			m_fileName;
	QString			m_lastError;
};
