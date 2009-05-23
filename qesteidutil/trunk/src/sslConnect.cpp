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

#include "sslConnect.h"

#include <stdexcept>
#include <sstream>

#ifdef WIN32
#include <winsock2.h>
#define SSL_LIB "ssleay32"
#define CRYPTO_LIB "libeay32"
#define PKCS11_ENGINE "engine_pkcs11"
#define PKCS11_MODULE "opensc-pkcs11.dll"
#else
#include <netdb.h>
#define SSL_LIB "ssl"
#define CRYPTO_LIB "crypto"
#define PKCS11_ENGINE "/usr/lib/engines/engine_pkcs11.so"
#define PKCS11_MODULE "/usr/lib/opensc-pkcs11.so"
#endif

#define EESTI "sisene.www.eesti.ee"

unsigned long (*pERR_get_error)(void);
void (*pERR_error_string_n)(unsigned long e,char *buf,size_t len);

class sslError:public std::runtime_error {
public:
    std::string desc;
    sslError(std::string op) : std::runtime_error("openssl error") {
		char buff[128] = "";
		std::ostringstream buf;
		pERR_error_string_n(pERR_get_error(),buff,sizeof(buff));
		buf << op << " (" << buff << ")";
        desc = buf.str();
        }
    ~sslError() throw () {}
    virtual const char * what() const throw() {	return desc.c_str();}
    void static check(std::string op,int result) {
        if (!result) throw sslError(op);
        }
    void static check(std::string op,void *result) {
        if (!result) throw sslError(op);
        }
};

SSLConnect::SSLConnect( int reader, QObject *parent )
:	QObject( parent )
,	obj( 0 )
,	m_reader( reader )
{}

SSLConnect::~SSLConnect()
{
	if ( obj )
		delete obj;
}

bool SSLConnect::isLoaded()
{
	return obj ? true : false;
}

std::vector<unsigned char> SSLConnect::getUrl( const std::string &type, const std::string &pin, const std::string &value )
{
	if ( !isLoaded() && !pin.length() )
		return std::vector<unsigned char>();

	if ( type == "picture" )
		return getSiteUrl( EESTI, "/idportaal/portaal.idpilt", pin );
	else if ( type == "emails" )
		return getSiteUrl( EESTI, "/idportaal/postisysteem.naita_suunamised", pin );
	else if ( type == "activateEmail" )
		return getSiteUrl( EESTI, "/idportaal/postisysteem.lisa_suunamine?" + value, pin );
	return std::vector<unsigned char>();
}

std::vector<unsigned char> SSLConnect::getSiteUrl( const std::string &site, const std::string &url, const std::string &pin )
{
	if ( !obj )
	{
		try { obj = new SSLObj( m_reader, pin ); }
		catch( std::runtime_error &e ) {
			if( obj )
				delete obj;
			obj = 0;
			throw sslError( e.what() );
		}
	}

	if ( !obj->connectToHost( site ) )
		return std::vector<unsigned char>();
	return obj->getUrl( url );
}

SSLObj::SSLObj( int reader, const std::string &pin )
:	engine( NULL )
,	ssl( new QLibrary( SSL_LIB ) )
,	e( new QLibrary( CRYPTO_LIB ) )
,	m_reader( reader )
{
    pSSL_library_init = (int (*)(void)) ssl->resolve("SSL_library_init");
	pSSL_load_error_strings = (void (*)(void)) ssl->resolve("SSL_load_error_strings");
    pSSL_CTX_new = (SSL_CTX * (*)(SSL_METHOD *meth)) ssl->resolve("SSL_CTX_new");
    pSSL_CTX_free = (void (*)(SSL_CTX *)) ssl->resolve("SSL_CTX_free");
    pSSL_CTX_use_PrivateKey = (int (*)(SSL_CTX *ctx, EVP_PKEY *pkey)) ssl->resolve("SSL_CTX_use_PrivateKey");
    pSSL_CTX_use_certificate = (int (*)(SSL_CTX *ctx, X509 *x)) ssl->resolve("SSL_CTX_use_certificate");
    pSSL_CTX_check_private_key = (int (*)(const SSL_CTX *ctx)) ssl->resolve("SSL_CTX_check_private_key");
	pSSL_CTX_ctrl = (long (*)( SSL_CTX *ctx, int cmd, long, void *)) ssl->resolve("SSL_CTX_ctrl");

	pSSLv3_method = (SSL_METHOD * (*)(void)) ssl->resolve("SSLv3_method");
	pSSL_new=(SSL * (*)(SSL_CTX *ctx)) ssl->resolve("SSL_new");
    pSSL_free=(void (*)(SSL *ssl)) ssl->resolve("SSL_free");
    pSSL_set_fd=(int (*)(SSL *s, int fd)) ssl->resolve("SSL_set_fd");
    pSSL_connect=(int (*)(SSL *ssl)) ssl->resolve("SSL_connect");
    pSSL_read=(int (*)(SSL *ssl,void *buf,int num)) ssl->resolve("SSL_read");
    pSSL_write=(int (*)(SSL *ssl,const void *buf,int num)) ssl->resolve("SSL_write");
    pSSL_shutdown=(int (*)(SSL *s)) ssl->resolve("SSL_shutdown");
	pSSL_get_error = (int (*)(const SSL *s,int ret_code)) ssl->resolve("SSL_get_error");

	pERR_get_error = (unsigned long (*)(void)) e->resolve("ERR_get_error");
    pERR_error_string_n = (void (*)(unsigned long e,char *buf,size_t len)) e->resolve("ERR_error_string_n");

	pENGINE_load_dynamic=(void (*)(void)) e->resolve("ENGINE_load_dynamic");
	pENGINE_by_id=(ENGINE * (*)(const char *id)) e->resolve("ENGINE_by_id");
	pENGINE_cleanup=(void (*)(void)) e->resolve("ENGINE_cleanup");
    pENGINE_init=(int (*)(ENGINE *e)) e->resolve("ENGINE_init");
    pENGINE_finish=(int (*)(ENGINE *e)) e->resolve("ENGINE_finish");
	pENGINE_free=(int (*)(ENGINE *e)) e->resolve("ENGINE_free");
    pENGINE_ctrl_cmd_string=(int (*)(ENGINE *e, const char *cmd_name, const char *arg,int cmd_optional)) e->resolve("ENGINE_ctrl_cmd_string");
	pENGINE_ctrl_cmd=(int (*)(ENGINE *e, const char *cmd_name, long i, void *p, void (*f)(void), int cmd_optional)) e->resolve("ENGINE_ctrl_cmd");

    pENGINE_load_private_key=(EVP_PKEY * (*)(ENGINE *e, const char *key_id, UI_METHOD *ui_method, void *callback_data)) e->resolve("ENGINE_load_private_key");

    pSSL_library_init();
    pSSL_load_error_strings();
	pENGINE_load_dynamic();

    //load and initialize pkcs11 engine
	sslError::check("get dynamic engine",engine = pENGINE_by_id("dynamic"));
    sslError::check("cmd SO_PATH",  pENGINE_ctrl_cmd_string(engine,"SO_PATH",PKCS11_ENGINE,0));
	sslError::check("cmd ID",       pENGINE_ctrl_cmd_string(engine,"ID","pkcs11",0));
	sslError::check("cmd LIST_ADD", pENGINE_ctrl_cmd_string(engine,"LIST_ADD","0",0));

	int result = pENGINE_ctrl_cmd_string(engine,"LOAD",NULL,0);
	if( !result )
		pENGINE_free( engine );
	sslError::check("cmd LOAD", result );

	sslError::check("cmd MODULE_PATH",pENGINE_ctrl_cmd_string(engine,"MODULE_PATH",PKCS11_MODULE,0));
	result = pENGINE_init(engine);
	if( !result )
		pENGINE_free( engine );
	sslError::check("init engine", result );

	std::ostringstream strm;
	strm << m_reader * 4 << ":01";
	std::string slotid = strm.str();
	struct {
		const char *slot_id;
		X509 *cert;
	} parms = {slotid.c_str(),NULL};

	sslError::check("cmd LOAD_CERT_CTRL",pENGINE_ctrl_cmd(engine, "LOAD_CERT_CTRL", 0, &parms, NULL, 0));

	struct
	{
		const void *password;
		const char *prompt_info;
	} cb_data = { pin.c_str(), NULL };

	EVP_PKEY *m_pkey = pENGINE_load_private_key(engine,parms.slot_id,NULL, &cb_data);
	sslError::check("wrong pin ?", m_pkey );

    ctx = pSSL_CTX_new( pSSLv3_method() );
	sslError::check("CTX_use_cert", pSSL_CTX_use_certificate(ctx, parms.cert ) );
	sslError::check("CTX_use_privkey", pSSL_CTX_use_PrivateKey(ctx,m_pkey) );
    sslError::check("CTX_check_privkey", pSSL_CTX_check_private_key(ctx) );
	sslError::check("CTX_ctrl", pSSL_CTX_ctrl( ctx, SSL_CTRL_MODE, SSL_MODE_AUTO_RETRY, NULL ) );

	s = pSSL_new(ctx);

#ifdef WIN32
    WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup( wVersionRequested, &wsaData );
#endif
}

SSLObj::~SSLObj()
{
	pSSL_shutdown(s);
	pSSL_free(s);
	pSSL_CTX_free(ctx);
	//crash
	//pENGINE_finish(engine);
	pENGINE_free(engine);
	pENGINE_cleanup();
	delete e;
	delete ssl;
}

bool SSLObj::connectToHost( const std::string &site )
{
	struct hostent *ent;
	if ( !(ent = gethostbyname(site.c_str()) ) )
		return false;

	unsigned long address = *((unsigned int *)*ent->h_addr_list);

	struct sockaddr_in server_addr;
	memset (&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
 	server_addr.sin_port        = htons(443);
	server_addr.sin_addr.s_addr = address;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int err = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));

	pSSL_set_fd(s, sock);
	sslError::check("SSL_connect", pSSL_connect(s) );
	return true;
}

std::vector<unsigned char> SSLObj::getUrl( const std::string &url )
{
	std::string req = "GET " + url + " HTTP/1.0\r\n\r\n";
    sslError::check("sslwrite", pSSL_write(s,req.c_str(),req.length()) );

	std::vector<unsigned char> buffer;
	buffer.resize(4096 * 4);
	int readChunk = 4096,bytesRead;
    size_t bytesTotal = 0;
	do {
	    if (bytesTotal > (buffer.size() - readChunk))
            buffer.resize(buffer.size() + (readChunk * 4));
        bytesRead = pSSL_read(s, &buffer[bytesTotal], readChunk);
        if (bytesRead != -1)
			bytesTotal+= bytesRead;
    } while(bytesRead > 0 );

    int lastErr = pSSL_get_error(s,bytesRead);
    if (lastErr != SSL_ERROR_NONE && lastErr != SSL_ERROR_ZERO_RETURN)
		sslError::check("SSL_write /GET failed", 0);

	buffer[bytesTotal] = '\0';
	buffer.erase(buffer.begin() + bytesTotal,buffer.end());
	std::string pageStr(buffer.size(),'\0');
	copy(buffer.begin(),buffer.end(),pageStr.begin());

    size_t pos = pageStr.find("\r\n\r\n");
    if (pos!= std::string::npos)
		buffer.erase(buffer.begin(),buffer.begin() + pos + 4);
	return buffer;
}
