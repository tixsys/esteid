#ifndef JSCARDMANAGER_H
#define JSCARDMANAGER_H

#include <QObject>
#include <QHash>
#include <QTimer>

#include "cardlib/common.h"
#include "cardlib/SmartCardManager.h"
#include "jsesteidcard.h"

class JsCardManager : public QObject
{
    Q_OBJECT

    struct ReaderState {
		QString name;
        bool connected;
		QString cardId;
		int id;
    };

public:
    JsCardManager(JsEsteidCard *jsEsteidCard);

private:
    SmartCardManager *cardMgr;
    JsEsteidCard *m_jsEsteidCard;
    QTimer pollTimer;

    QHash<QString,ReaderState> cardReaders;
    QString m_jsCardInsertFunc;
    QString m_jsCardRemoveFunc;
    QString m_jsHandleErrorFunc;

    void handleError(QString msg);

public slots:
    void findCard();
    int getReaderCount();
    QString getReaderName( int i );
    bool selectReader( int i );
    void registerCallBack( QString event, QString function );
	bool isInReader( QString cardId );

private slots:
    void pollCard();

signals:
    void cardEvent(QString func, int i);
    void cardError(QString func, QString err);
};

#endif // JSCARDMANAGER_H
