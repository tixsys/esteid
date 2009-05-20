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

#include <QtWebKit>

#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent )
:	QWebView( parent )
{
    setContextMenuPolicy(Qt::PreventContextMenu);
	setWindowIcon( QIcon( ":/html/images/id_icon_48x48.png" ) );
    setWindowTitle(tr("ID-card utility"));
	setFixedSize( 601, 520 );

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
