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

#include <QLibrary>
#include <QObject>

#include <openssl/ssl.h>

#include <vector>

class SSLObj
{

public:
	SSLObj( int reader, const std::string &pin );
	~SSLObj();

	bool connectToHost( const std::string &site );
	std::vector<unsigned char> getUrl( const std::string &type );

private:
	ENGINE *engine;
	SSL_CTX *ctx;
	SSL		*s;
	QLibrary *ssl, *e;
	int		m_reader;

	int     (*pSSL_library_init)(void );
    void    (*pSSL_load_error_strings)(void);
	int     (*pSSL_get_error)(const SSL *s,int ret_code);
	SSL_METHOD * (*pSSLv3_method)(void);
	SSL *   (*pSSL_new)(SSL_CTX *ctx);
    void    (*pSSL_free)(SSL *ssl);
    int     (*pSSL_set_fd)(SSL *s, int fd);
    int     (*pSSL_connect)(SSL *ssl);
    int     (*pSSL_read)(SSL *ssl,void *buf,int num);
    int     (*pSSL_write)(SSL *ssl,const void *buf,int num);
    int     (*pSSL_shutdown)(SSL *s);

	SSL_CTX * (*pSSL_CTX_new)(SSL_METHOD *meth);
	void    (*pSSL_CTX_free)(SSL_CTX *);
	long	(*pSSL_CTX_ctrl)(SSL_CTX *ctx, int cmd, long larg, void * );
	int     (*pSSL_CTX_use_PrivateKey)(SSL_CTX *ctx, EVP_PKEY *pkey);
    int     (*pSSL_CTX_use_certificate)(SSL_CTX *ctx, X509 *x);
    int     (*pSSL_CTX_check_private_key)(const SSL_CTX *ctx);

	void    (*pENGINE_load_dynamic)(void);
	ENGINE * (*pENGINE_by_id)(const char *id);
	void	(*pENGINE_cleanup)(void);
    int     (*pENGINE_init)(ENGINE *e);
    int     (*pENGINE_finish)(ENGINE *e);
	int     (*pENGINE_free)(ENGINE *e);
    int     (*pENGINE_ctrl_cmd_string)(ENGINE *e, const char *cmd_name, const char *arg,int cmd_optional);
	int     (*pENGINE_ctrl_cmd)(ENGINE *e, const char *cmd_name, long i, void *p, void (*f)(void), int cmd_optional);
	EVP_PKEY * (*pENGINE_load_private_key)(ENGINE *e, const char *key_id, UI_METHOD *ui_method, void *callback_data);
};

class SSLConnect: public QObject
{
	Q_OBJECT

public:
	SSLConnect( int reader, QObject *parent = 0 );
	~SSLConnect();

	bool isLoaded();
	std::vector<unsigned char> getUrl( const std::string &type, const std::string &pin = "", const std::string &value = "" );

private:
	SSLObj	*obj;
	int		m_reader;
	std::vector<unsigned char> getSiteUrl( const std::string &site, const std::string &url, const std::string &pin );
};
