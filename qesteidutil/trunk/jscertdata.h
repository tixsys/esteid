#ifndef JSCERTDATA_H
#define JSCERTDATA_H

#include <QString>
#include <QObject>
#include <QSslCertificate>

#include "cardlib/common.h"
#include "cardlib/EstEidCard.h"
#include "utility/asnObject.h"
#include "utility/asnCertificate.h"

class JsCertData : public QObject
{
    Q_OBJECT

public:
    JsCertData();

    enum CertType {
        AuthCert,
        SignCert
    };

    void loadCert(EstEidCard *card, CertType ct);

    QString email;
    QString subjCN;
    QString subjO;
    QString subjOU;
    QString validFrom;
    QString validTo;
    QString issuerCN;
    QString issuerO;
    QString issuerOU;

private:
    EstEidCard *m_card;
    QString fromX509Data(ByteVec val);
#ifndef QT_NO_OPENSSL
    QSslCertificate *m_qcert;
#endif

public slots:
    QString toPem();
    QString getEmail();
    QString getSubjCN();
    QString getSubjO();
    QString getSubjOU();
    QString getValidFrom();
    QString getValidTo();
    QString getIssuerCN();
    QString getIssuerO();
    QString getIssuerOU();
};

#endif // JSCERTDATA_H
