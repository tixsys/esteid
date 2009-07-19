/*
 * QEstEidCommon
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

#include <QDateTime>
#include <QSslKey>
#include <QStringList>

#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>

static QByteArray ASN_STRING_to_QByteArray( ASN1_OCTET_STRING *str )
{ return QByteArray( (const char *)ASN1_STRING_data(str), ASN1_STRING_length(str) ); }


SslCertificate::SslCertificate( const QSslCertificate &cert )
: QSslCertificate( cert ) {}

QByteArray SslCertificate::authorityKeyIdentifier() const
{
	AUTHORITY_KEYID *id = (AUTHORITY_KEYID *)getExtension( NID_authority_key_identifier );
	QByteArray out;
	if( id->keyid )
		out = ASN_STRING_to_QByteArray( id->keyid );
	AUTHORITY_KEYID_free( id );
	return out;
}

QString SslCertificate::decode( const QString &in ) const
{
	if( in.contains( "\\x" ) )
		return in.contains( "\\x00" ) ? toUtf16( in ) : toUtf8( in );
	else
		return in;
}

QStringList SslCertificate::enhancedKeyUsage() const
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
		case NID_OCSP_sign:
			list << QObject::tr("OCSP signing"); break;
		default: break;
		}
	}
	sk_ASN1_OBJECT_pop_free( usage, ASN1_OBJECT_free );

	if( list.isEmpty() )
		list << QObject::tr("All application policies");
	return list;
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

QSslCertificate SslCertificate::fromPKCS12( const QByteArray &data, const QByteArray &passPhrase )
{
	BIO *bio = BIO_new( BIO_s_mem() );
	if( !bio )
		return QSslCertificate();

	BIO_write( bio, data.data(), data.size() );

	PKCS12 *p12 = d2i_PKCS12_bio( bio, NULL );
	BIO_free( bio );
	if( !p12 )
		return QSslCertificate();

	X509 *cert;
	EVP_PKEY *key;
	int ret = PKCS12_parse( p12, passPhrase.data(), &key, &cert, NULL );
	PKCS12_free( p12 );

	if( !ret )
		return QSslCertificate();

	QSslCertificate c = fromX509( Qt::HANDLE(cert) );
	X509_free( cert );
	EVP_PKEY_free( key );

	return c;
}

QSslCertificate SslCertificate::fromX509( const Qt::HANDLE x509 )
{
	unsigned char *cert = NULL;
	int len = i2d_X509( (X509*)x509, &cert );
	QByteArray der;
	if( len >= 0 )
	{
		der = QByteArray( (char*)cert, len );
		OPENSSL_free( cert );
	}
	return QSslCertificate( der, QSsl::Der );
}

QSslKey SslCertificate::keyFromPKCS12( const QByteArray &data, const QByteArray &passPhrase )
{
	BIO *bio = BIO_new( BIO_s_mem() );
	if( !bio )
		return QSslKey();

	BIO_write( bio, data.data(), data.size() );

	PKCS12 *p12 = d2i_PKCS12_bio( bio, NULL );
	BIO_free( bio );
	if( !p12 )
		return QSslKey();

	X509 *cert;
	EVP_PKEY *key;
	int ret = PKCS12_parse( p12, passPhrase.data(), &key, &cert, NULL );
	PKCS12_free( p12 );

	if( !ret )
		return QSslKey();

	QSslKey c = keyFromEVP( Qt::HANDLE(key) );

	X509_free( cert );
	EVP_PKEY_free( key );

	return c;
}

QSslKey SslCertificate::keyFromEVP( const Qt::HANDLE evp )
{
	EVP_PKEY *key = (EVP_PKEY*)evp;
	unsigned char *data = NULL;
	int len = 0;
	QSsl::KeyAlgorithm alg;
	QSsl::KeyType type;

	switch( EVP_PKEY_type( key->type ) )
	{
	case EVP_PKEY_RSA:
	{
		RSA *rsa = EVP_PKEY_get1_RSA( key );
		alg = QSsl::Rsa;
		type = rsa->d ? QSsl::PrivateKey : QSsl::PublicKey;
		if( rsa->d )
			len = i2d_RSAPrivateKey( rsa, &data );
		else
			len = i2d_RSAPublicKey( rsa, &data );
		RSA_free( rsa );
		break;
	}
	case EVP_PKEY_DSA:
	{
		DSA *dsa = EVP_PKEY_get1_DSA( key );
		alg = QSsl::Dsa;
		type = dsa->priv_key ? QSsl::PrivateKey : QSsl::PublicKey;
		if( dsa->priv_key )
			len = i2d_DSAPrivateKey( dsa, &data );
		else
			len = i2d_DSAPublicKey( dsa, &data );
		DSA_free( dsa );
		break;
	}
	default: break;
	}

	QSslKey k;
	if( len > 0 )
	{
		k = QSslKey( QByteArray( (char*)data, len ), alg, QSsl::Der, type );
		free( data );
	}

	return k;
}

void* SslCertificate::getExtension( int nid ) const
{
	if( !handle() )
		return NULL;
	return X509_get_ext_d2i( (X509*)handle(), nid, NULL, NULL );
}

bool SslCertificate::isTempel() const
{
	Q_FOREACH( const QString &p, policies() )
		if( p.left( 19 ) == "1.3.6.1.4.1.10015.7" )
			return true;
	return false;
}

QHash<int,QString> SslCertificate::keyUsage() const
{
	QHash<int,QString> list;

	ASN1_BIT_STRING *keyusage = (ASN1_BIT_STRING*)getExtension( NID_key_usage );
	for( int n = 0; n < 9; ++n )
	{
		if( ASN1_BIT_STRING_get_bit( keyusage, n ) )
		{
			switch( n )
			{
			case DigitalSignature: list[n] = QObject::tr("Digital signature"); break;
			case NonRepudiation: list[n] = QObject::tr("Non repudiation"); break;
			case KeyEncipherment: list[n] = QObject::tr("Key encipherment"); break;
			case DataEncipherment: list[n] = QObject::tr("Data encipherment"); break;
			case KeyAgreement: list[n] = QObject::tr("Key agreement"); break;
			case KeyCertificateSign: list[n] = QObject::tr("Key certificate sign"); break;
			case CRLSign: list[n] = QObject::tr("CRL sign"); break;
			case EncipherOnly: list[n] = QObject::tr("Encipher only"); break;
			case DecipherOnly: list[n] = QObject::tr("Decipher only"); break;
			default: break;
			}
		}
	}
	ASN1_BIT_STRING_free( keyusage );

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
		if( len != NID_undef )
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
{ return decode( subjectInfo( subject ) ); }

QString SslCertificate::subjectInfoUtf8( const QByteArray &tag ) const
{ return decode( subjectInfo( tag ) ); }

QByteArray SslCertificate::subjectKeyIdentifier() const
{
	ASN1_OCTET_STRING *id = (ASN1_OCTET_STRING *)getExtension( NID_subject_key_identifier );
	QByteArray out = ASN_STRING_to_QByteArray( id );
	ASN1_OCTET_STRING_free( id );
	return out;
}

QByteArray SslCertificate::toHex( const QByteArray &in, QChar separator )
{
	QByteArray ret = in.toHex().toUpper();
	for( int i = 2; i < ret.size(); i += 3 )
		ret.insert( i, separator );
	return ret;
}

QDateTime SslCertificate::toLocalTime( const QDateTime &datetime )
{ return QDateTime( datetime.date(), datetime.time(), Qt::UTC ).toLocalTime(); }

QString SslCertificate::toUtf16( const QString &in ) const
{
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

QString SslCertificate::toUtf8( const QString &in ) const
{
	QString res;
	int i = 0;
	while( i < in.size() )
	{
		if( in.mid( i, 2 ) == "\\x" )
		{
			const char data[2] = {
				in.mid( i+2, 2 ).toUInt( 0, 16 ),
				in.mid( i+6, 2 ).toUInt( 0, 16 ) };
			res +=  QString::fromUtf8( data, 2 );
			i += 8;
		}
		else
		{
			res += in[i];
			++i;
		}
	}
	return res;
}

QByteArray SslCertificate::versionNumber() const
{
	if( !handle() )
		return QByteArray();
	return QByteArray::number( qlonglong(ASN1_INTEGER_get( ((X509*)handle())->cert_info->version )) + 1 );
}
