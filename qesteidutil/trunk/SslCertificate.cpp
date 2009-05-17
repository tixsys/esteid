/*
 * QEstEidUtil
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

#include "SslCertificate.h"

#include <QStringList>

#include <openssl/x509v3.h>

SslCertificate::SslCertificate( const QSslCertificate &cert )
: QSslCertificate( cert ) {}

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
	X509 *c = (X509*)handle();
	if( c != NULL )
	{
		int pos = X509_get_ext_by_NID( c, NID_ext_key_usage, -1 );
		if( pos != -1 )
		{
			X509_EXTENSION *ex = X509_get_ext( c, pos );
			if( ex != NULL )
			{
				EXTENDED_KEY_USAGE *usage = (EXTENDED_KEY_USAGE *)X509V3_EXT_d2i( ex );
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
			}
		}
	}

	if( list.isEmpty() )
		list << QObject::tr("All application policies");
	return list;
}

QByteArray SslCertificate::serialNumber() const
{
	if( !handle() )
		return false;
	return QByteArray::number( ASN1_INTEGER_get( ((X509*)handle())->cert_info->serialNumber ) );
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
		return false;
	return QByteArray::number( ASN1_INTEGER_get( ((X509*)handle())->cert_info->version ) + 1 );
}
