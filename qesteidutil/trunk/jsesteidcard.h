#ifndef JSESTEIDCARD_H
#define JSESTEIDCARD_H

#include <QString>
#include <QObject>

#include "cardlib/common.h"
#include "cardlib/SmartCardManager.h"
#include "cardlib/EstEidCard.h"
#include "jscertdata.h"

class JsEsteidCard : public QObject
{
    Q_OBJECT

public:
    JsEsteidCard();

    void setCard(EstEidCard *card);
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

    JsCertData *m_authCert;
    Q_PROPERTY(QObject* authCert READ getAuthCert)
    QObject *getAuthCert()
    {
        if (m_authCert == NULL)
            m_authCert = new JsCertData();
        return m_authCert;
    }

    JsCertData *m_signCert;
    Q_PROPERTY(QObject* signCert READ getSignCert)
    QObject *getSignCert()
    {
        if (m_signCert == NULL)
            m_signCert = new JsCertData();
        return m_signCert;
    }

private:
    SmartCardManager *m_cardManager;
    EstEidCard *m_card;
    void handleError(QString msg);
	dword authUsageCount;
	dword signUsageCount;

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

	QString parseName( const QString &in );

signals:
    void cardError(QString func, QString err);
};

#endif // JSESTEIDCARD_H
