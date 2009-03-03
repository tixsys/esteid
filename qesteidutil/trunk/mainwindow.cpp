#include <QtGui>
#include <QtWebKit>

#include "mainwindow.h"


MainWindow::MainWindow()
{
    webView = new QWebView;
    webView->setContextMenuPolicy(Qt::PreventContextMenu);
    setCentralWidget(webView);
    m_jsExtender = new JsExtender(webView->page()->mainFrame());

    jsEsteidCard = new JsEsteidCard();
    jsCardManager = new JsCardManager(jsEsteidCard);

    connect(jsCardManager, SIGNAL(cardEvent(QString, int)),
            m_jsExtender, SLOT(jsCall(QString, int)));
    connect(jsCardManager, SIGNAL(cardError(QString, QString)),
            m_jsExtender, SLOT(jsCall(QString, QString)));
    connect(jsEsteidCard, SIGNAL(cardError(QString, QString)),
            m_jsExtender, SLOT(jsCall(QString, QString)));

    jsCardManager->findCard();

    m_jsExtender->registerObject("esteidData", jsEsteidCard);
    m_jsExtender->registerObject("cardManager", jsCardManager);
//    webView->load(QUrl(QDir::currentPath() + "/html/index.html"));
    webView->load(QUrl("qrc:/html/index.html"));

    setWindowTitle(tr("ID-card utility"));
}
