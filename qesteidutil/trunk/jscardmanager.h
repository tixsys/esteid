#ifndef JSCARDMANAGER_H
#define JSCARDMANAGER_H

#include <QObject>
#include <QVector>
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
    };

public:
    JsCardManager(JsEsteidCard *jsEsteidCard);
    void findCard();

private:
    SmartCardManager *cardMgr;
    JsEsteidCard *m_jsEsteidCard;
    QTimer pollTimer;

    QVector<ReaderState> cardReaders;
    QString m_jsCardInsertFunc;
    QString m_jsCardRemoveFunc;
    QString m_jsHandleErrorFunc;

    void handleError(QString msg);

public slots:
    int getReaderCount();
    QString getReaderName(int i);
    bool selectReader(int i);
    void registerCallBack(QString event, QString function);

private slots:
    void pollCard();

signals:
    void cardEvent(QString func, int i);
    void cardError(QString func, QString err);
};

#endif // JSCARDMANAGER_H
