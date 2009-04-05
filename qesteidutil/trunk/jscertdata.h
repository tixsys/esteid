#ifndef JSCERTDATA_H
#define JSCERTDATA_H

#include <QString>
#include <QObject>
#include <QSslCertificate>

#include "cardlib/common.h"
#include "cardlib/EstEidCard.h"

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

private:
    EstEidCard *m_card;
    QSslCertificate *m_qcert;

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
