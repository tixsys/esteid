#pragma once

#include <QSslCertificate>

class SslCertificate: public QSslCertificate
{
public:
	SslCertificate( const QSslCertificate &cert );
	QByteArray serialNumber() const;
	QByteArray versionNumber() const;
};
