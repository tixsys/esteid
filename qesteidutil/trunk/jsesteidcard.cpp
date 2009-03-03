#include <iostream>
#include <QDebug>

#include "jsesteidcard.h"

using namespace std;

JsEsteidCard::JsEsteidCard()
{
    m_card = NULL;
    m_authCert = new JsCertData();
    m_signCert = new JsCertData();
}

void JsEsteidCard::setCard(EstEidCard *card)
{
    m_card = card;
    m_authCert->loadCert(card, JsCertData::AuthCert);
    m_signCert->loadCert(card, JsCertData::SignCert);
    reloadData();
}

void JsEsteidCard::handleError(QString msg)
{
    qDebug() << "Error: " << msg << endl;
    // XXX
//    emit cardError(m_jsHandleErrorFunc, msg);
    emit cardError("handleError", msg);
}

void JsEsteidCard::reloadData() {
    if (!m_card) {
        qDebug("No card");
        return;
    }

    std::vector<std::string> tmp;
    try {
        // Read all personal data
        m_card->readPersonalData(tmp,EstEidCard::SURNAME,EstEidCard::COMMENT4);

        surName = tmp[EstEidCard::SURNAME].c_str();
        firstName = tmp[EstEidCard::FIRSTNAME].c_str();
        middleName = tmp[EstEidCard::MIDDLENAME].c_str();
        sex = tmp[EstEidCard::SEX].c_str();
        citizen = tmp[EstEidCard::CITIZEN].c_str();
        birthDate = tmp[EstEidCard::BIRTHDATE].c_str();
        id = tmp[EstEidCard::ID].c_str();
        documentId = tmp[EstEidCard::DOCUMENTID].c_str();
        expiry = tmp[EstEidCard::EXPIRY].c_str();
        birthPlace = tmp[EstEidCard::BIRTHPLACE].c_str();
        issueDate = tmp[EstEidCard::ISSUEDATE].c_str();
        residencePermit = tmp[EstEidCard::RESIDENCEPERMIT].c_str();
        comment1 = tmp[EstEidCard::COMMENT1].c_str();
        comment2 = tmp[EstEidCard::COMMENT2].c_str();
        comment3 = tmp[EstEidCard::COMMENT3].c_str();
        comment4 = tmp[EstEidCard::COMMENT4].c_str();
    } catch (runtime_error &err ) {
//        doShowError(err);
        cout << "Error: " << err.what() << endl;
    }
}


bool JsEsteidCard::changePin1(QString newVal, QString oldVal)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
        return m_card->changeAuthPin(newVal.toStdString(),
                                     oldVal.toStdString(),
                                     retriesLeft);
    } catch(AuthError &err) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

bool JsEsteidCard::changePin2(QString newVal, QString oldVal)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
        return m_card->changeSignPin(newVal.toStdString(),
                                     oldVal.toStdString(),
                                     retriesLeft);
    } catch(AuthError &err) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

bool JsEsteidCard::changePuk(QString newVal, QString oldVal)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
        return m_card->changePUK(newVal.toStdString(),
                                 oldVal.toStdString(),
                                 retriesLeft);
    } catch(AuthError &err) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

bool JsEsteidCard::unblockPin1(QString newVal, QString puk)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
        return m_card->unblockAuthPin(newVal.toStdString(),
                                      puk.toStdString(),
                                      retriesLeft);
    } catch(AuthError &err) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

bool JsEsteidCard::unblockPin2(QString newVal, QString puk)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
        return m_card->unblockSignPin(newVal.toStdString(),
                                      puk.toStdString(),
                                      retriesLeft);
    } catch(AuthError &err) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

QString JsEsteidCard::getSurName()
{
    return surName;
}

QString JsEsteidCard::getFirstName()
{
    return firstName;
}

QString JsEsteidCard::getMiddleName()
{
    return middleName;
}

QString JsEsteidCard::getSex()
{
    return sex;
}

QString JsEsteidCard::getCitizen()
{
    return citizen;
}

QString JsEsteidCard::getBirthDate()
{
    return birthDate;
}

QString JsEsteidCard::getId()
{
    return id;
}

QString JsEsteidCard::getDocumentId()
{
    return documentId;
}

QString JsEsteidCard::getExpiry()
{
    return expiry;
}

QString JsEsteidCard::getBirthPlace()
{
    return birthPlace;
}

QString JsEsteidCard::getIssueDate()
{
    return issueDate;
}

QString JsEsteidCard::getResidencePermit()
{
    return residencePermit;
}

QString JsEsteidCard::getComment1()
{
    return comment1;
}

QString JsEsteidCard::getComment2()
{
    return comment2;
}

QString JsEsteidCard::getComment3()
{
    return comment3;
}

QString JsEsteidCard::getComment4()
{
    return comment4;
}

int JsEsteidCard::getPin1RetryCount()
{
    if (!m_card)
        return -1;

    byte puk,pinAuth,pinSign;
    m_card->getRetryCounts(puk,pinAuth,pinSign);
    return pinAuth;
}

int JsEsteidCard::getPin2RetryCount()
{
    if (!m_card)
        return -1;

    byte puk,pinAuth,pinSign;
    m_card->getRetryCounts(puk,pinAuth,pinSign);
    return pinSign;
}

int JsEsteidCard::getPukRetryCount()
{
    if (!m_card)
        return -1;

    byte puk,pinAuth,pinSign;
    m_card->getRetryCounts(puk,pinAuth,pinSign);
    return puk;
}
