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

	void lock();
	void selectCard( const QString &card );
	void unlock();

Q_SIGNALS:
	void dataChanged( const QStringList &cards, const QString &card,
		const QSslCertificate &auth, const QSslCertificate &sign );

private:
	void readCerts();
	void run();

	volatile bool terminate;
	QMutex m;
	QHash<QString,int> cards;
	QString selectedCard;
	QSslCertificate auth, sign;
};
