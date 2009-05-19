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

#include "SslCertificate.h"

#include <QStringList>

#include <openssl/x509v3.h>

SslCertificate::SslCertificate( const QSslCertificate &cert )
: QSslCertificate( cert ) {}

void* SslCertificate::getExtension( int nid ) const
{
	X509 *c = (X509*)handle();
	if( c != NULL )
		NULL;
	return X509_get_ext_d2i( c, nid, NULL, NULL );
}

QString SslCertificate::formatName( const QString &name )
{
	QString ret = name.toLower();
	bool firstChar = true;
	for( QString::iterator i = ret.begin(); i != ret.end(); ++i )
	{
		if( !firstChar && !i->isLetter() )
			firstChar = true;

		if( firstChar && i->isLetter() )
		{
			*i = i->toUpper();
			firstChar = false;
		}
	}
	return ret;
}

QStringList SslCertificate::keyUsage() const
{
	QStringList list;

	EXTENDED_KEY_USAGE *usage = (EXTENDED_KEY_USAGE*)getExtension( NID_ext_key_usage );
	for( int i = 0; i < sk_ASN1_OBJECT_num( usage ); ++i )
	{
		ASN1_OBJECT *obj = sk_ASN1_OBJECT_value( usage, i );
		switch( OBJ_obj2nid( obj ) )
		{
		case NID_client_auth:
			list << QObject::tr("Proves your identity to a remote computer"); break;
		case NID_email_protect:
			list << QObject::tr("Protects e-mail messages"); break;
		default: break;
		}
	}
	sk_ASN1_OBJECT_pop_free( usage, ASN1_OBJECT_free );

	if( list.isEmpty() )
		list << QObject::tr("All application policies");
	return list;
}

QStringList SslCertificate::policies() const
{
	QStringList list;

	CERTIFICATEPOLICIES *cp = (CERTIFICATEPOLICIES*)getExtension( NID_certificate_policies );
	for( int i = 0; i < sk_POLICYINFO_num( cp ); ++i )
	{
		POLICYINFO *pi = sk_POLICYINFO_value( cp, i );
		char buf[50];
		memset( buf, 0, 50 );
		int len = OBJ_obj2txt( buf, 50, pi->policyid, 1 );
		list << buf;
	}
	sk_POLICYINFO_pop_free( cp, POLICYINFO_free );

	return list;
}

QString SslCertificate::policyInfo( const QString &index ) const
{
#if 0
	for( int j = 0; j < sk_POLICYQUALINFO_num( pi->qualifiers ); ++j )
	{
		POLICYQUALINFO *pqi = sk_POLICYQUALINFO_value( pi->qualifiers, j );

		memset( buf, 0, 50 );
		int len = OBJ_obj2txt( buf, 50, pqi->pqualid, 1 );
		qDebug() << buf;
	}
#endif
	return QString();
}

QByteArray SslCertificate::serialNumber() const
{
	if( !handle() )
		return QByteArray();
	return QByteArray::number( qlonglong(ASN1_INTEGER_get( ((X509*)handle())->cert_info->serialNumber )) );
}

QString SslCertificate::subjectInfoUtf8( SubjectInfo subject ) const
{ return toUtf8( subjectInfo( subject ) ); }

QString SslCertificate::subjectInfoUtf8( const QByteArray &tag ) const
{ return toUtf8( subjectInfo( tag ) ); }

QString SslCertificate::toUtf8( const QString &in ) const
{
	if( !in.contains( "\\x" ) )
		return in;

	QString res;
	bool firstByte = true;
	ushort data;
	int i = 0;
	while( i < in.size() )
	{
		if( in.mid( i, 2 ) == "\\x" )
		{
			if( firstByte )
				data = in.mid( i+2, 2 ).toUShort( 0, 16 );
			else
				res += QChar( (data<<8) + in.mid( i+2, 2 ).toUShort( 0, 16 ) );
			i += 4;
		}
		else
		{
			if( firstByte )
				data = in[i].unicode();
			else
				res += QChar( (data<<8) + in[i].unicode() );
			++i;
		}
		firstByte = !firstByte;
	}
	return res;
}

QByteArray SslCertificate::versionNumber() const
{
	if( !handle() )
		return QByteArray();
	return QByteArray::number( qlonglong(ASN1_INTEGER_get( ((X509*)handle())->cert_info->version )) + 1 );
}
