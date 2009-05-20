/*
 * QEstEidUtil
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

#include <QSslCertificate>

class SslCertificate: public QSslCertificate
{
public:
	SslCertificate( const QSslCertificate &cert );

	static QString formatName( const QString &name );
	bool		isTempel() const;
	QStringList keyUsage() const;
	QStringList policies() const;
	QString		policyInfo( const QString &oid ) const;
	QString		subjectInfoUtf8( SubjectInfo subject ) const;
	QString		subjectInfoUtf8( const QByteArray &tag ) const;
	QByteArray	serialNumber() const;
	QByteArray	versionNumber() const;

private:
	void*	getExtension( int nid ) const;
	QString	toUtf8( const QString &data ) const;
};
