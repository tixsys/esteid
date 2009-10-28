/*
 * QEstEidCommon
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

#include "sslConnect.h"

#include <QThread>

#include <libp11.h>

#include <openssl/ssl.h>

class SSLObj
{
public:
	SSLObj();
	~SSLObj();

	bool connectToHost( SSLConnect::RequestType type );
	QByteArray getUrl( const QString &url ) const;
	QByteArray getRequest( const QString &request ) const;

	PKCS11_CTX *ctx;
	SSL		*s;

	QString card;
	QString pin;
	QString pkcs11;
	int		reader;

	unsigned int nslots;
	PKCS11_SLOT *pslots;
};

class SSLThread: public QThread
{
	Q_OBJECT
public:
	SSLThread( PKCS11_SLOT *slot, QObject *parent = 0 );
	~SSLThread();
	int loginOk;

private:
	void run();

	PKCS11_SLOT *m_slot;
};
