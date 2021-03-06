/*
 * QEstEidCommon
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
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

class SSLConnectPrivate
{
public:
	SSLConnectPrivate();
	~SSLConnectPrivate();

	bool connectToHost( SSLConnect::RequestType type );
	QByteArray getRequest( const QString &request ) const;

	bool	unload;
	PKCS11_CTX *p11;
	bool	p11loaded;
	SSL_CTX *sctx;
	SSL		*ssl;

	QString card;
	QString pin;
	int		reader;

	unsigned int nslots;
	PKCS11_SLOT *pslots;

	SSLConnect::ErrorType error;
	QString errorString;
	QByteArray result;
};

class SSLThread: public QThread
{
	Q_OBJECT
public:
	SSLThread( PKCS11_SLOT *slot, QObject *parent = 0 );
	~SSLThread();
	unsigned long loginResult;

private:
	void run();

	PKCS11_SLOT *m_slot;
};
