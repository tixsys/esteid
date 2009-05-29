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

#ifdef OPENSSL_SYS_WIN32 
#undef X509_NAME 
#undef X509_EXTENSIONS 
#undef X509_CERT_PAIR 
#undef PKCS7_ISSUER_AND_SERIAL 
#undef OCSP_REQUEST 
#undef OCSP_RESPONSE 
#endif 

#include <openssl/ssl.h>
#include <openssl/engine.h>

#define EESTI "sisene.www.eesti.ee"

class sslError:public std::runtime_error {
public:
    std::string desc;
    sslError(std::string op) : std::runtime_error("openssl error") {
		char buff[128] = "";
		std::ostringstream buf;
		ERR_error_string_n(ERR_get_error(),buff,sizeof(buff));
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

SSLConnect::SSLConnect( QObject *parent )
:	QObject( parent )
,	obj( 0 )
{
	try { obj = new SSLObj(); }
	catch( sslError &e ) { clear(); throw e; }
	catch( std::runtime_error &e ) { clear(); throw e; }
}

SSLConnect::~SSLConnect() { clear(); }

void SSLConnect::clear()
{
	if ( obj )
		delete obj;
}

bool SSLConnect::isLoaded() { return obj; }

std::vector<unsigned char> SSLConnect::getUrl( const std::string &pin, int readerNum, const std::string &type, const std::string &value )
{
	if ( !isLoaded() )
		return std::vector<unsigned char>();

	if ( obj->connectToHost( EESTI, pin, readerNum ) )
	{
		if ( type == "picture" )
			return obj->getUrl( "/idportaal/portaal.idpilt" );
		if ( type == "emails" )
			return obj->getUrl( "/idportaal/postisysteem.naita_suunamised" );
		if ( type == "activateEmail" )
			return obj->getUrl( "/idportaal/postisysteem.lisa_suunamine?" + value );
	}
	obj->disconnect();
	return std::vector<unsigned char>();
}

SSLObj::SSLObj()
{
	engine = (ENGINE*)malloc( sizeof( ENGINE* ) );

    SSL_library_init();
    SSL_load_error_strings();
	ENGINE_load_dynamic();

    //load and initialize pkcs11 engine
	sslError::check("get dynamic engine",engine = ENGINE_by_id("dynamic"));
    sslError::check("cmd SO_PATH",  ENGINE_ctrl_cmd_string(engine,"SO_PATH",PKCS11_ENGINE,0));
	sslError::check("cmd ID",       ENGINE_ctrl_cmd_string(engine,"ID","pkcs11",0));
	sslError::check("cmd LIST_ADD", ENGINE_ctrl_cmd_string(engine,"LIST_ADD","1",0));

	int result = ENGINE_ctrl_cmd_string(engine,"LOAD",NULL,0);
	if( !result )
		ENGINE_free( engine );
	sslError::check("cmd LOAD", result );

	sslError::check("cmd MODULE_PATH",ENGINE_ctrl_cmd_string(engine,"MODULE_PATH",PKCS11_MODULE,0));

	sslError::check("set_default", ENGINE_set_default(engine, (unsigned int)0xFFF) );

	result = ENGINE_init(engine);
	if( !result )
		ENGINE_free( engine );
	sslError::check("init engine", result );
}

void SSLObj::disconnect()
{
	SSL_shutdown(s);
	SSL_free(s);
	SSL_CTX_free(ctx);
}

SSLObj::~SSLObj()
{
	ENGINE_finish(engine);
	ENGINE_free(engine);
	ENGINE_cleanup();
}

bool SSLObj::connectToHost( const std::string &site, const std::string &pin, int reader )
{
	std::ostringstream strm;
	strm << reader * 4 << ":01";
	std::string slotid = strm.str();
	struct {
		const char *slot_id;
		X509 *cert;
	} parms = {slotid.c_str(),NULL};

	sslError::check("cmd LOAD_CERT_CTRL",ENGINE_ctrl_cmd(engine, "LOAD_CERT_CTRL", 0, &parms, NULL, 0));

	struct
	{
		const void *password;
		const char *prompt_info;
	} cb_data = { pin.c_str(), NULL };

	EVP_PKEY *m_pkey = ENGINE_load_private_key(engine,parms.slot_id,NULL, &cb_data);
	sslError::check("wrong pin ?", m_pkey );

    ctx = SSL_CTX_new( SSLv3_method() );
	sslError::check("CTX_use_cert", SSL_CTX_use_certificate(ctx, parms.cert ) );
	sslError::check("CTX_use_privkey", SSL_CTX_use_PrivateKey(ctx,m_pkey) );
    sslError::check("CTX_check_privkey", SSL_CTX_check_private_key(ctx) );
	sslError::check("CTX_ctrl", SSL_CTX_ctrl( ctx, SSL_CTRL_MODE, SSL_MODE_AUTO_RETRY, NULL ) );
	if ( m_pkey )
		EVP_PKEY_free(m_pkey); 
	s = SSL_new(ctx);

#ifdef WIN32
    WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAStartup( wVersionRequested, &wsaData );
#endif

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

	SSL_set_fd(s, sock);
	sslError::check("SSL_connect", SSL_connect(s) );
	return true;
}

std::vector<unsigned char> SSLObj::getUrl( const std::string &url )
{
	std::string req = "GET " + url + " HTTP/1.0\r\n\r\n";
    sslError::check("sslwrite", SSL_write(s,req.c_str(),req.length()) );

	std::vector<unsigned char> buffer;
	buffer.resize(4096 * 4);
	int readChunk = 4096,bytesRead;
    size_t bytesTotal = 0;
	do {
	    if (bytesTotal > (buffer.size() - readChunk))
            buffer.resize(buffer.size() + (readChunk * 4));
        bytesRead = SSL_read(s, &buffer[bytesTotal], readChunk);
        if (bytesRead != -1)
			bytesTotal+= bytesRead;
    } while(bytesRead > 0 );

    int lastErr = SSL_get_error(s,bytesRead);
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
