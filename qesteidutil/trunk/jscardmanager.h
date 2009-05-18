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
		QString state;
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
    int getReaderCount();
    QString getReaderName( int i );
    bool selectReader( int i );
	bool selectReader( const ReaderState &reader );
    void registerCallBack( QString event, QString function );
	bool isInReader( const QString &cardId );
	void showDiagnostics();

private slots:
    void pollCard();

signals:
    void cardEvent(QString func, int i);
    void cardError(QString func, QString err);
};

#endif // JSCARDMANAGER_H
