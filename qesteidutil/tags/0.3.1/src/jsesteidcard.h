/*
 * QEstEidUtil
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
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

#include <smartcardpp/PCSCManager.h>
#include <smartcardpp/esteid/EstEidCard.h>
#include "jscertdata.h"

class JsEsteidCard : public QObject
{
    Q_OBJECT

public:
    JsEsteidCard( QObject *parent );

    void setCard(EstEidCard *card, int reader = 0);
    void reloadData();
	void reconnect();

    EstEidCard *m_card;
	int			m_reader;

	JsCertData *m_authCert;
    Q_PROPERTY(QObject* authCert READ getAuthCert)
		QObject *getAuthCert() { return m_authCert; }

    JsCertData *m_signCert;
    Q_PROPERTY(QObject* signCert READ getSignCert)
		QObject *getSignCert() { return m_signCert; }

public slots:
    QString getSurName();
    QString getFirstName();
    QString getMiddleName();
    QString getSex();
    QString getCitizen();
    QString getBirthDate( const QString &locale = "en" );
    QString getId();
    QString getDocumentId();
    QString getExpiry( const QString &locale = "en" );
    QString getBirthPlace();
    QString getIssueDate();
    QString getResidencePermit();
    QString getComment1();
    QString getComment2();
    QString getComment3();
    QString getComment4();

	bool canReadCard();
	bool isValid();

    int getPin1RetryCount( bool connect = false );
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

private:
	PCSCManager *m_cardManager;
	void handleError(QString msg);
	dword authUsageCount;
	dword signUsageCount;

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

	bool cardOK;
};
