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

#pragma once

#include <QObject>
#include <QHash>
#include <QTimer>

#include "cardlib/common.h"
#include "cardlib/SmartCardManager.h"
#include "jsesteidcard.h"
#include "sslConnect.h"

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
	SSLConnect* ssl() { return jsSSL; }

private:
    SmartCardManager *cardMgr;
	SSLConnect *jsSSL;
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
