#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <QStringList>

#include "jscertdata.h"

using namespace std;

JsCertData::JsCertData()
{
    m_card = NULL;
}

void JsCertData::loadCert(EstEidCard *card, CertType ct)
{
    m_card = card;

    if (!m_card) {
        qDebug("No card");
        return;
    }

    std::vector<std::string> tmp;
    ByteVec certBytes;
    try {
        // Read certificate
        if (ct == AuthCert)
            certBytes = m_card->getAuthCert();
        else
            certBytes = m_card->getSignCert();

        m_qcert = new QSslCertificate(QByteArray((char *)&certBytes[0], certBytes.size()), QSsl::Der);
    } catch (runtime_error &err ) {
//        doShowError(err);
        cout << "Error: " << err.what() << endl;
    }
}

QString JsCertData::toPem()
{
    if (!m_qcert)
        return "";

    return QString(m_qcert->toPem());
}

QString JsCertData::getEmail()
{
    if (!m_qcert)
        return "";

    QStringList mailaddresses = m_qcert->alternateSubjectNames().values(QSsl::EmailEntry);

    // return first email address
    if (!mailaddresses.isEmpty())
        return mailaddresses.first();
    else
        return "";
}

QString JsCertData::getSubjCN()
{
    if (!m_qcert)
        return "";

    return m_qcert->subjectInfo(QSslCertificate::CommonName);
}

QString JsCertData::getSubjO()
{
    if (!m_qcert)
        return "";

    return m_qcert->subjectInfo(QSslCertificate::Organization);
}

QString JsCertData::getSubjOU()
{
    if (!m_qcert)
        return "";

    return m_qcert->subjectInfo(QSslCertificate::OrganizationalUnitName);
}

QString JsCertData::getValidFrom()
{
    if (!m_qcert)
        return "";

    return m_qcert->effectiveDate().toString("dd.MM.yyyy");
}

QString JsCertData::getValidTo()
{
    if (!m_qcert)
        return "";

    return m_qcert->expiryDate().toString("dd.MM.yyyy");
}

QString JsCertData::getIssuerCN()
{
    if (!m_qcert)
        return "";

    return m_qcert->issuerInfo(QSslCertificate::CommonName);
}

QString JsCertData::getIssuerO()
{
    if (!m_qcert)
        return "";

    return m_qcert->issuerInfo(QSslCertificate::Organization);
}

QString JsCertData::getIssuerOU()
{
    if (!m_qcert)
        return "";

    return m_qcert->issuerInfo(QSslCertificate::OrganizationalUnitName);
}
