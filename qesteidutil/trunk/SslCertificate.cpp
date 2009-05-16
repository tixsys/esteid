#include "SslCertificate.h"

#include <openssl/X509.h>

SslCertificate::SslCertificate( const QSslCertificate &cert )
: QSslCertificate( cert ) {}

QByteArray SslCertificate::serialNumber() const
{ return QByteArray::number( ASN1_INTEGER_get( ((X509*)handle())->cert_info->serialNumber ) ); }

QByteArray SslCertificate::versionNumber() const
{ return QByteArray::number( ASN1_INTEGER_get( ((X509*)handle())->cert_info->version ) + 1 ); }
