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

#include <iostream>
#include <QDebug>
#include <QDate>
#include <QLocale>

#include "jsesteidcard.h"
#include "common/CertificateWidget.h"
#include "common/SslCertificate.h"

using namespace std;

JsEsteidCard::JsEsteidCard( QObject *parent )
:	QObject( parent )
{
    m_card = NULL;
    m_authCert = new JsCertData( this );
    m_signCert = new JsCertData( this );
	authUsageCount = 0;
	signUsageCount = 0;
}

void JsEsteidCard::setCard(EstEidCard *card, int reader)
{
	if ( m_card )
	{
		delete m_card;
		m_card = 0;
	}

	m_reader = reader;
    m_card = card;
	m_authCert->loadCert(card, JsCertData::AuthCert);
	m_signCert->loadCert(card, JsCertData::SignCert);
    reloadData();
}

void JsEsteidCard::handleError(QString msg)
{
    qDebug() << "Error: " << msg << endl;
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

        surName = SslCertificate::formatName( tmp[EstEidCard::SURNAME].c_str() );
        firstName = SslCertificate::formatName( tmp[EstEidCard::FIRSTNAME].c_str() );
        middleName = SslCertificate::formatName( tmp[EstEidCard::MIDDLENAME].c_str() );
        sex = tmp[EstEidCard::SEX].c_str();
        citizen = tmp[EstEidCard::CITIZEN].c_str();
        birthDate = tmp[EstEidCard::BIRTHDATE].c_str();
        id = tmp[EstEidCard::ID].c_str();
        documentId = tmp[EstEidCard::DOCUMENTID].c_str();
        expiry = tmp[EstEidCard::EXPIRY].c_str();
        birthPlace = SslCertificate::formatName( tmp[EstEidCard::BIRTHPLACE].c_str() );
        issueDate = tmp[EstEidCard::ISSUEDATE].c_str();
        residencePermit = tmp[EstEidCard::RESIDENCEPERMIT].c_str();
        comment1 = tmp[EstEidCard::COMMENT1].c_str();
        comment2 = tmp[EstEidCard::COMMENT2].c_str();
        comment3 = tmp[EstEidCard::COMMENT3].c_str();
        comment4 = tmp[EstEidCard::COMMENT4].c_str();

		m_card->getKeyUsageCounters( authUsageCount, signUsageCount);
	} catch (runtime_error &err ) {
        cout << "Error: " << err.what() << endl;
    }
}

bool JsEsteidCard::canReadCard()
{
	return m_card && m_authCert && m_signCert && m_authCert->m_qcert && m_signCert->m_qcert;
}

bool JsEsteidCard::validatePin1(QString oldVal)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
		return m_card->validateAuthPin(oldVal.toStdString(), retriesLeft);
    } catch(AuthError &) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
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
	} catch(AuthError &) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

bool JsEsteidCard::validatePin2(QString oldVal)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
		return m_card->validateSignPin(oldVal.toStdString(),
										retriesLeft);
    } catch(AuthError &) {
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
	} catch(AuthError &) {
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
        return false;
    }
}

bool JsEsteidCard::validatePuk(QString oldVal)
{
    if (!m_card) {
        qDebug("No card");
        return false;
    }

    byte retriesLeft = 0;

    try {
		return m_card->validatePuk(oldVal.toStdString(),
                                     retriesLeft);
    } catch(AuthError &) {
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
    } catch(AuthError &) {
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
    } catch(AuthError &) {
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
	} catch(AuthError &) {
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

QString JsEsteidCard::getBirthDate( const QString &locale )
{
    return QLocale( locale ).toString( QDate::fromString( birthDate, "dd.MM.yyyy" ), "dd. MMMM yyyy" );
}

QString JsEsteidCard::getId()
{
    return id;
}

QString JsEsteidCard::getDocumentId()
{
    return documentId;
}

QString JsEsteidCard::getExpiry( const QString &locale )
{
	return QLocale( locale ).toString( QDate::fromString( expiry, "dd.MM.yyyy" ), "dd. MMMM yyyy" );
}

QString JsEsteidCard::getBirthPlace()
{
	return birthPlace;
}

QString JsEsteidCard::getIssueDate()
{
    return QDate::fromString( issueDate, "dd.MM.yyyy" ).toString( "dd. MMMM yyyy" );
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
    
	byte puk = -1;
	byte pinAuth = -1;
	byte pinSign = -1;

	try {
		m_card->getRetryCounts(puk,pinAuth,pinSign);
	} catch ( std::runtime_error &e ) {
		qDebug() << e.what();
	}

    return pinAuth;
}

int JsEsteidCard::getPin2RetryCount()
{
    if (!m_card)
        return -1;

	byte puk = -1;
	byte pinAuth = -1;
	byte pinSign = -1;

	try {
		m_card->getRetryCounts(puk,pinAuth,pinSign);
	} catch ( std::runtime_error &e ) {
		qDebug() << e.what();
	}

    return pinSign;
}

int JsEsteidCard::getPukRetryCount()
{
    if (!m_card)
        return -1;

	byte puk = -1;
	byte pinAuth = -1;
	byte pinSign = -1;

	try {
		m_card->getRetryCounts(puk,pinAuth,pinSign);
	} catch ( std::runtime_error &e ) {
		qDebug() << e.what();
	}
    return puk;
}

int JsEsteidCard::getAuthUsageCount()
{
	return authUsageCount;
}

int JsEsteidCard::getSignUsageCount()
{
	return signUsageCount;
}

bool JsEsteidCard::isValid()
{
	if (!m_card)
		return false;

	return QDateTime::fromString( expiry, "dd.MM.yyyy" ) >= QDateTime::currentDateTime();
}

bool JsEsteidCard::checkPin( const QString &pin )
{
	QDate date( QDate::fromString( birthDate, "dd.MM.yyyy" ) );
    if ( pin.contains( date.toString( "yyyy" ) ) || 
			pin.contains( date.toString( "ddMM" ) ) ||
			pin.contains( date.toString( "MMdd" ) ) )
		return false;
	return true;
}

void JsEsteidCard::showCert( int type )
{
	CertificateDialog *c = new CertificateDialog;
	if( type == 1 )
		c->setCertificate( m_authCert->cert() );
	else
		c->setCertificate( m_signCert->cert() );
	c->show();
}

void JsEsteidCard::reconnect()
{
	if (!m_card)
		return;

	try {
		m_card->connect( m_reader, true );
	} catch ( std::runtime_error &e ) {
		qDebug() << e.what();
	}
}
