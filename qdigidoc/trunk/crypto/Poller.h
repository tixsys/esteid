/*
 * QDigiDocCrypto
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

#include <QThread>

#include <QHash>
#include <QMutex>
#include <QSslCertificate>

class Poller: public QThread
{
	Q_OBJECT

public:
	Poller( QObject *parent = 0 );
	~Poller();

	quint64 slot( const QString &card ) const;
	quint64 token( const QString &card ) const;
	void stop();

Q_SIGNALS:
	void dataChanged( const QStringList &cards, const QString &card,
		const QSslCertificate &auth );

private Q_SLOTS:
	void selectCard( const QString &card );

private:
	void read();
	void readCert();
	void run();

	Qt::HANDLE lib;
	volatile bool terminate;
	QMutex m;
	QHash<QString,quint64> cards, tokens;
	QString selectedCard, select;
	QSslCertificate auth;
};
