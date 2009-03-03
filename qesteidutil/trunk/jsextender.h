#ifndef JSEXTENDER_H
#define JSEXTENDER_H

#include <QObject>
#include <QtWebKit>

class JsExtender : public QObject
{
    Q_OBJECT

public:
    JsExtender(QWebFrame *frame);
    void connectSignals(QWebFrame *frame);
    void registerObject(const QString &name, QObject *object);

private:
    QWebFrame *m_webFrame;
    QMap<QString, QObject *> m_registeredObjects;

public slots:
    void javaScriptWindowObjectCleared();
    QVariant jsCall(const QString &function, int argument);
    QVariant jsCall(const QString &function, const QString &argument);
};

#endif // JSEXTENDER_H
