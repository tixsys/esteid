#include <QtWebKit>

#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent )
:	QWebView( parent )
{
    setContextMenuPolicy(Qt::PreventContextMenu);
	setWindowIcon( QIcon( ":/html/images/id_icon_48x48.png" ) );

	m_jsExtender = new JsExtender( this );

    jsEsteidCard = new JsEsteidCard( this );
    jsCardManager = new JsCardManager( jsEsteidCard );

    connect(jsCardManager, SIGNAL(cardEvent(QString, int)),
            m_jsExtender, SLOT(jsCall(QString, int)));
    connect(jsCardManager, SIGNAL(cardError(QString, QString)),
            m_jsExtender, SLOT(jsCall(QString, QString)));
    connect(jsEsteidCard, SIGNAL(cardError(QString, QString)),
            m_jsExtender, SLOT(jsCall(QString, QString)));

    jsCardManager->findCard();

    m_jsExtender->registerObject("esteidData", jsEsteidCard);
    m_jsExtender->registerObject("cardManager", jsCardManager);

	load(QUrl("qrc:/html/index.html"));

    setWindowTitle(tr("ID-card utility"));
	setFixedSize( 601, 520 );
}
