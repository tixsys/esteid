/*!
	\file		opensslObj.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-07 08:35:50 +0300 (Tue, 07 Jul 2009) $
*/
// Revision $Revision: 346 $

#pragma once
#include <smartcard++/DynamicLibrary.h>
#include "config.h"

#if HAVE_OPENSSL
#include <openssl/ssl.h>
#include <openssl/ui.h>
#include <stdint.h>

class opensslObj : public DynamicLibrary {
    friend struct wrapCTX;
    friend struct wrapSSL;
    friend struct opensslConnect;
protected:
	ENGINE *engine;

    int     (*pSSL_library_init)(void );
    void    (*pSSL_load_error_strings)(void);
    SSL_CTX * (*pSSL_CTX_new)(SSL_METHOD *meth);
    void    (*pSSL_CTX_free)(SSL_CTX *);

    SSL_METHOD * (*pSSLv3_method)(void);
    int     (*pSSL_CTX_use_PrivateKey)(SSL_CTX *ctx, EVP_PKEY *pkey);
    int     (*pSSL_CTX_use_certificate)(SSL_CTX *ctx, X509 *x);
    int     (*pSSL_CTX_check_private_key)(const SSL_CTX *ctx);

    SSL *   (*pSSL_new)(SSL_CTX *ctx);
    void    (*pSSL_free)(SSL *ssl);
    int     (*pSSL_set_fd)(SSL *s, int fd);
    int     (*pSSL_connect)(SSL *ssl);
    int     (*pSSL_read)(SSL *ssl,void *buf,int num);
    int     (*pSSL_write)(SSL *ssl,const void *buf,int num);
    int     (*pSSL_shutdown)(SSL *s);
    const char * (*pSSL_CIPHER_get_name)(const SSL_CIPHER *c);
    SSL_CIPHER * (*pSSL_get_current_cipher)(const SSL *s);
    int     (*pSSL_get_error)(const SSL *s,int ret_code);

    void    (*pENGINE_load_builtin_engines)(void);
    ENGINE * (*pENGINE_by_id)(const char *id);
    int     (*pENGINE_init)(ENGINE *e);
    int     (*pENGINE_finish)(ENGINE *e);
    int     (*pENGINE_ctrl_cmd_string)(ENGINE *e, const char *cmd_name,
                const char *arg,int cmd_optional);
    int     (*pENGINE_ctrl_cmd)(ENGINE *e, const char *cmd_name,
                long i, void *p, void (*f)(void), int cmd_optional);
    EVP_PKEY * (*pENGINE_load_private_key)(ENGINE *e, const char *key_id,
                UI_METHOD *ui_method, void *callback_data);
    UI_METHOD *(*pUI_create_method)(char *name);
    int (*pUI_method_set_reader)(UI_METHOD *method, int (*reader)(UI *ui, UI_STRING *uis));
private:
    X509 * m_cert;
	EVP_PKEY *m_pkey;
    void * m_appPtr;
public:
    opensslObj(void *app);
    ~opensslObj();
    void init(int readerNum);
    X509 * getCert() const { return m_cert;}
	EVP_PKEY * getPrivKey() const {return m_pkey;};
};

struct wrapCTX {
    opensslObj &ossl;
    SSL_CTX * ctx;
    wrapCTX(opensslObj &ref);
    ~wrapCTX();
    operator SSL_CTX*() const {return ctx;}
    };

struct opensslConnect {
    opensslObj &ossl;
    int sock;
    SSL *ssl;
    wrapCTX ctx;
    std::string ciphername,sitename,cookieStr;
    std::string req,headers;
    int lastErr;
    uint32_t aadress;

    opensslConnect(opensslObj &ref,std::string site,connType type,void *);
    ~opensslConnect();
	bool getHttpsFile(std::string url,std::vector<unsigned char> &buffer);
	void sopen();
	void sclose();
	};
#else
class opensslObj : public DynamicLibrary {
public:
    opensslObj(void *app);
    void init(int ) {}
};

struct opensslConnect {
    opensslConnect(opensslObj &ref,std::string site,connType type,void *) {}
    bool getHttpsFile(std::string url,std::vector<unsigned char> &buffer) {return false;}
    };
#endif
