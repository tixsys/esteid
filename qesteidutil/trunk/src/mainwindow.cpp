/*
 * QEstEidUtil
 *
 * Copyright (C) 2009 Jargo Kõster <jargo@innovaatik.ee>
 * Copyright (C) 2009 Raul Metsma <raul@innovaatik.ee>
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

#include <QApplication>
#include <QtWebKit>

#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent )
:	QWebView( parent )
{
	setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint );
#if QT_VERSION >= 0x040500
	setWindowFlags( windowFlags() | Qt::WindowCloseButtonHint );
#else
	setWindowFlags( windowFlags() | Qt::WindowSystemMenuHint );
#endif
	page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
	page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
	setContextMenuPolicy(Qt::PreventContextMenu);
	setWindowIcon( QIcon( ":/html/images/id_icon_48x48.png" ) );
	setFixedSize( 585, 535 );

	appTranslator = new QTranslator( this );
	qtTranslator = new QTranslator( this );
	QApplication::instance()->installTranslator( appTranslator );
	QApplication::instance()->installTranslator( qtTranslator );

	m_jsExtender = new JsExtender( this );
	
    jsEsteidCard = new JsEsteidCard( this );
    jsCardManager = new JsCardManager( jsEsteidCard );

    connect(jsCardManager, SIGNAL(cardEvent(QString, int)),
            m_jsExtender, SLOT(jsCall(QString, int)));
    connect(jsCardManager, SIGNAL(cardError(QString, QString)),
            m_jsExtender, SLOT(jsCall(QString, QString)));
    connect(jsEsteidCard, SIGNAL(cardError(QString, QString)),
            m_jsExtender, SLOT(jsCall(QString, QString)));

    m_jsExtender->registerObject("esteidData", jsEsteidCard);
    m_jsExtender->registerObject("cardManager", jsCardManager);

	load(QUrl("qrc:/html/index.html"));
}

void MainWindow::retranslate( const QString &lang )
{
	appTranslator->load( ":/translations/" + lang );
	qtTranslator->load( ":/translations/qt_" + lang );
	setWindowTitle(QApplication::translate("MainWindow", "ID-card utility", 0, QApplication::UnicodeUTF8));
}
