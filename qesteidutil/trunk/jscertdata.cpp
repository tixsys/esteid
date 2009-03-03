#include <iostream>
#include <QDebug>

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

        std::stringstream dummy;
        //              std::ofstream decodeLog("decode.log");
        asnCertificate cert(certBytes,dummy);
#ifndef QT_NO_OPENSSL
        m_qcert = new QSslCertificate(QByteArray((char *)&certBytes[0], certBytes.size()), QSsl::Der);
#endif
        if (ct == AuthCert)
            email = cert.getSubjectAltName().c_str();
        subjCN    = fromX509Data(cert.getSubjectCN());
        subjO     = fromX509Data(cert.getSubjectO());
        subjOU    = fromX509Data(cert.getSubjectOU());
        validFrom = cert.getValidFrom().c_str();
        validTo   = cert.getValidTo().c_str();
        issuerCN  = fromX509Data(cert.getIssuerCN());
        issuerO   = fromX509Data(cert.getIssuerO());
        issuerOU  = fromX509Data(cert.getIssuerOU());
    } catch (runtime_error &err ) {
//        doShowError(err);
        cout << "Error: " << err.what() << endl;
    }
}

QString JsCertData::toPem()
{
#ifndef QT_NO_OPENSSL
    return QString(m_qcert->toPem());
#else
    return "";
#endif
}

QString JsCertData::fromX509Data(ByteVec val) {
    QString ret;
    if (val.end() != std::find(val.begin(), val.end(), 0)) { //contains zeroes, is bigendian UCS
        for(ByteVec::iterator it = val.begin(); it < val.end(); it += 2)
            iter_swap(it, it+1);
        val.push_back(0);
        val.push_back(0);
        return QString((const char *)&val[0]);
    } else {
        return QString((const char *)&val[0]).left(val.size());
    }
}

QString JsCertData::getEmail()
{
    return email;
}

QString JsCertData::getSubjCN()
{
    return subjCN;
}

QString JsCertData::getSubjO()
{
    return subjO;
}

QString JsCertData::getSubjOU()
{
    return subjOU;
}

QString JsCertData::getValidFrom()
{
    return validFrom;
}

QString JsCertData::getValidTo()
{
    return validTo;
}

QString JsCertData::getIssuerCN()
{
    return issuerCN;
}

QString JsCertData::getIssuerO()
{
    return issuerO;
}

QString JsCertData::getIssuerOU()
{
    return issuerOU;
}
