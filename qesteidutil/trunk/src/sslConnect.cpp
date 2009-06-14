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
#define PKCS11_MODULE "opensc-pkcs11.dll"
#else
#include <netdb.h>
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
	catch( sslError &e ) { throw e; }
	catch( std::runtime_error &e ) { throw e; }
}

SSLConnect::~SSLConnect()
{
	if ( obj )
	{
		delete obj;
		obj = 0;
	}
}

bool SSLConnect::isLoaded() { return obj && obj->ctx; }

std::vector<unsigned char> SSLConnect::getUrl( const std::string &pin, int readerNum, const std::string &type, const std::string &value )
{
	if ( !obj )
		obj = new SSLObj();

	std::vector<unsigned char> result;

	if ( !isLoaded() )
		return result;

	if ( obj->connectToHost( EESTI, pin, readerNum ) )
	{
		if ( type == "picture" )
			result =  obj->getUrl( "/idportaal/portaal.idpilt" );
		if ( type == "emails" )
			result = obj->getUrl( "/idportaal/postisysteem.naita_suunamised" );
		if ( type == "activateEmail" )
			result = obj->getUrl( "/idportaal/postisysteem.lisa_suunamine?" + value );
	}
	return result;
}

SSLObj::SSLObj()
{
	ctx = PKCS11_CTX_new();
	if ( PKCS11_CTX_load(ctx, PKCS11_MODULE) )
		throw std::runtime_error( QObject::tr( "failed to load pkcs11 module" ).toStdString() );
}

SSLObj::~SSLObj()
{
	SSL_shutdown(s);
	SSL_free(s);
	PKCS11_release_all_slots(ctx, pslots, nslots);
	PKCS11_CTX_unload(ctx);
	PKCS11_CTX_free(ctx);
}

bool SSLObj::connectToHost( const std::string &site, const std::string &pin, int reader )
{
	PKCS11_SLOT *slot;
	PKCS11_CERT *certs;
	PKCS11_KEY *authkey;
	PKCS11_CERT *authcert;
	EVP_PKEY *pubkey = NULL;
	unsigned int ncerts;

	int result = PKCS11_enumerate_slots(ctx, &pslots, &nslots);
	if ( result )
		throw std::runtime_error( QObject::tr( "failed to list slots" ).toStdString() );

	if ( (unsigned int)reader*4 > nslots )
		throw std::runtime_error( QObject::tr( "token failed" ).toStdString() );

	slot = &pslots[reader*4];
	if (!slot || !slot->token)
	{
		PKCS11_release_all_slots(ctx, pslots, nslots);
		throw std::runtime_error( QObject::tr("no token available").toStdString() );
	}

	result = PKCS11_enumerate_certs(slot->token, &certs, &ncerts);
	authcert=&certs[0];
	result = PKCS11_login(slot, 0, pin.c_str());
	authkey = PKCS11_find_key(authcert);
	if ( !authkey )
	{
		PKCS11_release_all_slots(ctx, pslots, nslots);
		throw std::runtime_error( QObject::tr("no key matching certificate available").toStdString() );
	}

	EVP_PKEY *pkey = PKCS11_get_private_key( authkey );

	SSL_CTX *sctx = SSL_CTX_new( SSLv3_method() );
	sslError::check("CTX_use_cert", SSL_CTX_use_certificate(sctx, authcert->x509 ) );
	sslError::check("CTX_use_privkey", SSL_CTX_use_PrivateKey(sctx,pkey) );
    sslError::check("CTX_check_privkey", SSL_CTX_check_private_key(sctx) );
	sslError::check("CTX_ctrl", SSL_CTX_ctrl( sctx, SSL_CTRL_MODE, SSL_MODE_AUTO_RETRY, NULL ) );
	if ( pkey != NULL )
		EVP_PKEY_free(pkey); 

	s = SSL_new(sctx);

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
