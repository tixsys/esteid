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

#pragma once

#include <QString>
#include <QObject>

#include "cardlib/common.h"
#include "cardlib/SmartCardManager.h"
#include "cardlib/EstEidCard.h"
#include "jscertdata.h"

class CertificateWidget;

class JsEsteidCard : public QObject
{
    Q_OBJECT

public:
    JsEsteidCard( QObject *parent );
	~JsEsteidCard();

    void setCard(EstEidCard *card, int reader = 0);
    void reloadData();

    QString surName;
    QString firstName;
    QString middleName;
    QString sex;
    QString citizen;
    QString birthDate;
    QString id;
    QString documentId;
    QString expiry;
    QString birthPlace;
    QString issueDate;
    QString residencePermit;
    QString comment1;
    QString comment2;
    QString comment3;
    QString comment4;

    EstEidCard *m_card;
	int			m_reader;

	JsCertData *m_authCert;
    Q_PROPERTY(QObject* authCert READ getAuthCert)
    QObject *getAuthCert()
    {
        if (m_authCert == NULL)
            m_authCert = new JsCertData( this );
        return m_authCert;
    }

    JsCertData *m_signCert;
    Q_PROPERTY(QObject* signCert READ getSignCert)
    QObject *getSignCert()
    {
        if (m_signCert == NULL)
            m_signCert = new JsCertData( this );
        return m_signCert;
    }

private:
    SmartCardManager *m_cardManager;
    void handleError(QString msg);
	dword authUsageCount;
	dword signUsageCount;
	CertificateWidget *certificateWidget;

public slots:
    QString getSurName();
    QString getFirstName();
    QString getMiddleName();
    QString getSex();
    QString getCitizen();
    QString getBirthDate();
    QString getId();
    QString getDocumentId();
    QString getExpiry();
    QString getBirthPlace();
    QString getIssueDate();
    QString getResidencePermit();
    QString getComment1();
    QString getComment2();
    QString getComment3();
    QString getComment4();

	bool canReadCard();

    int getPin1RetryCount();
    int getPin2RetryCount();
    int getPukRetryCount();
	
	int getAuthUsageCount();
	int getSignUsageCount();

    bool changePin1(QString newVal, QString oldVal);
    bool changePin2(QString newVal, QString oldVal);
    bool changePuk(QString newVal, QString oldVal);
    bool unblockPin1(QString newVal, QString puk);
    bool unblockPin2(QString newVal, QString puk);
	bool validatePin1(QString oldVal);
	bool validatePin2(QString oldVal);
	bool validatePuk(QString oldVal);
	bool checkPin( const QString &pin );

	void showCert( int type );

signals:
    void cardError(QString func, QString err);
};
