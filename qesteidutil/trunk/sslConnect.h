#pragma once

#include <QObject>

#include <openssl/ssl.h>
#include <openssl/ui.h>

#include <sstream>
#include <vector>
#include <QDebug>

class SSLObj
{

public:
	SSLObj( const std::string &pin );
	~SSLObj();

	void connectToHost( const std::string &site );
	std::vector<unsigned char> getUrl( const std::string &type );

private:
	ENGINE *engine;
	SSL_CTX *ctx;
	SSL		*s;

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

	void    (*pENGINE_load_builtin_engines)(void);
	ENGINE * (*pENGINE_by_id)(const char *id);
    int     (*pENGINE_init)(ENGINE *e);
    int     (*pENGINE_finish)(ENGINE *e);
    int     (*pENGINE_ctrl_cmd_string)(ENGINE *e, const char *cmd_name, const char *arg,int cmd_optional);
	int     (*pENGINE_ctrl_cmd)(ENGINE *e, const char *cmd_name, long i, void *p, void (*f)(void), int cmd_optional);
	EVP_PKEY * (*pENGINE_load_private_key)(ENGINE *e, const char *key_id, UI_METHOD *ui_method, void *callback_data);
};

class SSLConnect: public QObject
{
	Q_OBJECT

public:
	SSLConnect( QObject *parent = 0 );

	bool isLoaded();
	std::vector<unsigned char> getUrl( const std::string &type, const std::string &pin = "", const std::string &value = "" );

private:
	SSLObj *obj;

	std::vector<unsigned char> getSiteUrl( const std::string &site, const std::string &url, const std::string &pin );
};
