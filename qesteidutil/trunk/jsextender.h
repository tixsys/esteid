#ifndef JSEXTENDER_H
#define JSEXTENDER_H

#include <QObject>
#include <QtWebKit>
#include <QXmlStreamReader>

#include "sslConnect.h"

class JsExtender : public QObject
{
    Q_OBJECT

public:
    JsExtender(QWebFrame *frame);
	~JsExtender();
    void connectSignals(QWebFrame *frame);
    void registerObject(const QString &name, QObject *object);

private:
    QWebFrame *m_webFrame;
    QMap<QString, QObject *> m_registeredObjects;
	SSLConnect *jsSSL;
	QString m_tempFile;
	QXmlStreamReader xml;

public slots:
    void javaScriptWindowObjectCleared();
    QVariant jsCall(const QString &function, int argument);
    QVariant jsCall(const QString &function, const QString &argument);
	QVariant jsCall(const QString &function, const QString &argument, const QString &argument2);
	QString checkPin();
	void openUrl( const QString &url );
	void activateEmail( const QString &email );
	void loadEmails();
	QString readEmailAddresses();
	QString readForwards();
	void loadPicture();
};

#endif // JSEXTENDER_H
