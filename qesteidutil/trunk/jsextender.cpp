#include <QtWebKit>

#include "jsextender.h"

JsExtender::JsExtender(QWebFrame *frame)
{
    connectSignals(frame);
    m_webFrame = frame;
}

void JsExtender::connectSignals(QWebFrame *frame)
{
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(javaScriptWindowObjectCleared()));
}

void JsExtender::registerObject(const QString &name, QObject *object)
{
    m_webFrame->addToJavaScriptWindowObject(name, object);

    m_registeredObjects[name] = object;
}

void JsExtender::javaScriptWindowObjectCleared()
{
    for (QMap<QString, QObject *>::Iterator it =
                               m_registeredObjects.begin();
                               it != m_registeredObjects.end(); ++it) {
        QString name = it.key();
        QObject *object = it.value();
        m_webFrame->addToJavaScriptWindowObject(name, object);
    }

    m_webFrame->addToJavaScriptWindowObject("extender", this);
}

QVariant JsExtender::jsCall(const QString &function, int argument)
{
    QString statement = function + "(";
    statement += QString::number(argument);
    statement += ")";

    return m_webFrame->evaluateJavaScript(statement);
}

QVariant JsExtender::jsCall(const QString &function, const QString &argument)
{
    QString statement = function + "(";
    statement += "\"";
    statement += argument;
    statement += "\"";
    statement += ")";

    return m_webFrame->evaluateJavaScript(statement);
}
