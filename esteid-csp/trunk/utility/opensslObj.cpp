/*!
	\file		opensslObj.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-17 16:19:22 +0300 (Fri, 17 Apr 2009) $
*/
// Revision $Revision: 255 $

#include "precompiled.h"
#include "netObj.h"
#include "opensslObj.h"

#if HAVE_OPENSSL 

#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/engine.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <algorithm>

void * (*pUI_get_ex_data)(UI *r, int idx);
int (*pUI_get_result_maxsize)(UI_STRING *uis);
const char *(*pUI_get0_output_string)(UI_STRING *uis);
int (*pUI_set_result)(UI *ui, UI_STRING *uis, const char *result);

unsigned long (*pERR_get_error)(void);
void (*pERR_error_string_n)(unsigned long e,char *buf,size_t len);

class sslError:public std::runtime_error {
public:
    int code;
    std::string desc;
    sslError(std::string op) : std::runtime_error("openssl error") {
	char buff[128] = "";
	std::ostringstream buf;
	pERR_error_string_n(pERR_get_error(),buff,sizeof(buff));
	buf << op << " (" << buff << ")";
        desc = buf.str();
        }
    sslError(std::string op,int res) : std::runtime_error("ossl error") {
        desc = op + ":" + strerror(res);
        }
    ~sslError() throw () {}
    virtual const char * what() const throw() {	return desc.c_str();}
    void static check(std::string op,int result) {
        if (!result) throw sslError(op);
        }
    void static check(std::string op,void *result) {
        if (!result) throw sslError(op);
        }
    void static checkerr(std::string op,int res) {
        if (res == -1) throw sslError(op,res);
        }

    };

//provided by application, first param gets application pointer back
void getPassword(void *,std::string prompt,int maxinput,std::string &pin);

static int my_ui_method_read(UI *ui, UI_STRING *uis)
{
	int maxlen = pUI_get_result_maxsize(uis);
	const char *prompt = pUI_get0_output_string(uis);
	std::string pin;
	getPassword(pUI_get_ex_data(ui,0),prompt,maxlen,pin);
	pUI_set_result(ui, uis, pin.c_str());
	return 1;
}

//most distros dont have "libssl" symlink, so try exact versions
//order of approximate likelyhood ?
const char *sslLibName() {
	const char *arrNames[] = {"libssl.so.0.9.8", "libssl.so.0.9.9","libssl.so.0.9.7"};
	for (int i = 0; i < 3; i++)
		try {
			DynamicLibrary l(arrNames[i]);
			return arrNames[i];
		} catch(std::runtime_error &) {}
	return "ssl";
	}

opensslObj::opensslObj(void *app) :
	DynamicLibrary(sslLibName()),engine(NULL),pENGINE_finish(NULL),m_appPtr(app) {
    pSSL_library_init = (int (*)(void)) getProc("SSL_library_init");
    pSSL_load_error_strings = (void (*)(void))getProc("SSL_load_error_strings");

    pSSL_CTX_new = (SSL_CTX * (*)(SSL_METHOD *meth)) getProc("SSL_CTX_new");
    pSSL_CTX_free = (void (*)(SSL_CTX *)) getProc("SSL_CTX_free");

    pERR_get_error = (unsigned long (*)(void)) getProc("ERR_get_error");
    pERR_error_string_n = (void (*)(unsigned long e,char *buf,size_t len)) getProc("ERR_error_string_n");

    pSSLv3_method = (SSL_METHOD * (*)(void)) getProc("SSLv3_method");
    pSSL_CTX_use_PrivateKey = (int (*)(SSL_CTX *ctx, EVP_PKEY *pkey))
        getProc("SSL_CTX_use_PrivateKey");
    pSSL_CTX_use_certificate = (int (*)(SSL_CTX *ctx, X509 *x))
        getProc("SSL_CTX_use_certificate");
    pSSL_CTX_check_private_key = (int (*)(const SSL_CTX *ctx))
        getProc("SSL_CTX_check_private_key");

    pSSL_new=(SSL * (*)(SSL_CTX *ctx)) getProc("SSL_new");
    pSSL_free=(void (*)(SSL *ssl)) getProc("SSL_free");
    pSSL_set_fd=(int (*)(SSL *s, int fd)) getProc("SSL_set_fd");
    pSSL_connect=(int (*)(SSL *ssl)) getProc("SSL_connect");
    pSSL_read=(int (*)(SSL *ssl,void *buf,int num)) getProc("SSL_read");
    pSSL_write=(int (*)(SSL *ssl,const void *buf,int num)) getProc("SSL_write");
    pSSL_shutdown=(int (*)(SSL *s)) getProc("SSL_shutdown");

    pSSL_CIPHER_get_name = (const char * (*)(const SSL_CIPHER *c)) getProc("SSL_CIPHER_get_name");
    pSSL_get_current_cipher = (SSL_CIPHER * (*)(const SSL *s)) getProc("SSL_get_current_cipher");

    pSSL_get_error = (int (*)(const SSL *s,int ret_code)) getProc("SSL_get_error");

    pENGINE_load_builtin_engines=(void (*)(void))
        getProc("ENGINE_load_builtin_engines");
    pENGINE_by_id=(ENGINE * (*)(const char *id))
        getProc("ENGINE_by_id");
    pENGINE_init=(int (*)(ENGINE *e))
        getProc("ENGINE_init");
    pENGINE_finish=(int (*)(ENGINE *e))
        getProc("ENGINE_finish");
    pENGINE_ctrl_cmd_string=(int (*)(ENGINE *e, const char *cmd_name,
                const char *arg,int cmd_optional))
            getProc("ENGINE_ctrl_cmd_string");
    pENGINE_ctrl_cmd=(int (*)(ENGINE *e, const char *cmd_name,
                long i, void *p, void (*f)(void), int cmd_optional))
            getProc("ENGINE_ctrl_cmd");
    pENGINE_load_private_key=(EVP_PKEY * (*)(ENGINE *e, const char *key_id,
                UI_METHOD *ui_method, void *callback_data))
        getProc("ENGINE_load_private_key");

    pUI_create_method =(UI_METHOD* (*)(char *name)) getProc("UI_create_method");
    pUI_method_set_reader = (int (*)(UI_METHOD *method, int (*reader)(UI *ui, UI_STRING *uis)))
        getProc("UI_method_set_reader");

    pUI_get_ex_data = (void * (*)(UI *r, int idx)) getProc("UI_get_ex_data");
    pUI_get_result_maxsize =(int (*)(UI_STRING *uis)) getProc("UI_get_result_maxsize");
    pUI_get0_output_string = (const char *(*)(UI_STRING *uis)) getProc("UI_get0_output_string");
    pUI_set_result = (int (*)(UI *ui, UI_STRING *uis, const char *result)) getProc("UI_set_result");

    pSSL_library_init();
    pSSL_load_error_strings();
    pENGINE_load_builtin_engines();

	DynamicLibrary opensc_pkcs("opensc-pkcs11");
	DynamicLibrary pkcs_engine("engine_pkcs11","engines");

    //load and initialize pkcs11 engine
	sslError::check("get dynamic engine",engine = pENGINE_by_id("dynamic"));
    sslError::check("cmd SO_PATH",  pENGINE_ctrl_cmd_string(engine,"SO_PATH",
                                        pkcs_engine.getVersionStr().c_str(),0));
	sslError::check("cmd ID",       pENGINE_ctrl_cmd_string(engine,"ID","pkcs11",0));
	sslError::check("cmd LIST_ADD", pENGINE_ctrl_cmd_string(engine,"LIST_ADD","0",0));
	sslError::check("cmd LOAD",     pENGINE_ctrl_cmd_string(engine,"LOAD",NULL,0));
//sslError::check("cmd VERBOSE",pENGINE_ctrl_cmd_string(engine,"VERBOSE",0,0));
	sslError::check("cmd MODULE_PATH",pENGINE_ctrl_cmd_string(engine,"MODULE_PATH",
                                        opensc_pkcs.getVersionStr().c_str(),0));
	sslError::check("init engine",  pENGINE_init(engine));
    }

void opensslObj::init(int readerNum) {
//slotid: "slot_0-id_1","slot_4-id_1","slot_8-id_1", depending on readers
    std::ostringstream strm;
    strm << "slot_" << readerNum * 4 << //EstEid lists 4 slots per reader..
        "-id_1";
    std::string slotid = strm.str();
	struct {
		const char * slot_id;
		X509 * cert;
	} parms = {slotid.c_str(),NULL};

	sslError::check("cmd LOAD_CERT_CTRL",pENGINE_ctrl_cmd(engine, "LOAD_CERT_CTRL",
		0, &parms, NULL, 0));
    m_cert = parms.cert;

	UI_METHOD * ui_method = NULL;
	ui_method = pUI_create_method((char *)"myssl ui meth");
	pUI_method_set_reader(ui_method, my_ui_method_read);

	sslError::check("wrong pin ?",
        m_pkey = pENGINE_load_private_key(engine,parms.slot_id,ui_method, m_appPtr));
	}

opensslObj::~opensslObj() {
	if (engine && pENGINE_finish) pENGINE_finish(engine);
	}

wrapCTX::wrapCTX(opensslObj &ref) : ossl(ref) {
    SSL_METHOD *meth = ossl.pSSLv3_method();
    ctx = ossl.pSSL_CTX_new(meth);
	ossl.pSSL_CTX_use_certificate(ctx,ossl.getCert());
	ossl.pSSL_CTX_use_PrivateKey(ctx,ossl.getPrivKey());
    ossl.pSSL_CTX_check_private_key(ctx);
    }

wrapCTX::~wrapCTX() {
    ossl.pSSL_CTX_free(ctx);
    }

opensslConnect::opensslConnect(opensslObj &ref,std::string site,connType type,void *) :
    ossl(ref),ctx(ref),sitename(site)
    {
    if (type != HTTPS ) throw std::runtime_error("HTTP unsupported");

	struct hostent *ent;
	ent = gethostbyname(site.c_str());
	if (!ent->h_length) throw std::runtime_error("name resolution failed");

	aadress = *((unsigned int *)*ent->h_addr_list);
    }

void opensslConnect::sopen() {
	struct sockaddr_in server_addr;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset (&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
 	server_addr.sin_port        = htons(443);
	server_addr.sin_addr.s_addr = aadress;

	int err = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
	sslError::checkerr("connect",err);

	ssl = ossl.pSSL_new(ctx);
	ossl.pSSL_set_fd(ssl, sock);
	err = ossl.pSSL_connect(ssl);
	if (err == -1) throw std::runtime_error("no connection");
//	throw std::runtime_error("bla");
	ciphername = ossl.pSSL_CIPHER_get_name(ossl.pSSL_get_current_cipher(ssl));
    }

void opensslConnect::sclose() {
    int err = ossl.pSSL_shutdown(ssl);
    err = close(sock);
    ossl.pSSL_free(ssl);
    }

opensslConnect::~opensslConnect() {
//    SSL_free(ssl);
    }

typedef std::vector<unsigned char> ByteVec;
#include <iostream>
bool opensslConnect::getHttpsFile(std::string url,std::vector<unsigned char> &buffer) {
    sopen();
    /*std::string */req = "GET " + url + " HTTP/1.1\r\n" +
        "Host: " + sitename + "\r\n";
    if (cookieStr.length() > 0) req+= "Cookie:" + cookieStr + "\r\n";
    req+= "\r\n";
    int err = ossl.pSSL_write(ssl,req.c_str(),req.length());
    sslError::checkerr("sslwrite",err);
    buffer.resize(4096 * 4);
	int readChunk = 4096,bytesRead;
    size_t bytesTotal = 0;
	do {
	    if (bytesTotal > (buffer.size() - readChunk))
            buffer.resize(buffer.size() + (readChunk * 4));

            bytesRead = ossl.pSSL_read(ssl, &buffer[bytesTotal], readChunk);
            if (bytesRead != -1) bytesTotal+= bytesRead;
    } while(bytesRead > 0 );
    lastErr = ossl.pSSL_get_error(ssl,bytesRead);
    if (lastErr != SSL_ERROR_NONE && lastErr != SSL_ERROR_ZERO_RETURN)
        throw std::runtime_error("badcode");

	buffer[bytesTotal] = '\0';
	buffer.erase(buffer.begin() + bytesTotal,buffer.end());
	std::string pageStr(buffer.size(),'\0');
	copy(buffer.begin(),buffer.end(),pageStr.begin());
	size_t pos = pageStr.find("Set-Cookie:");
    size_t posn = pageStr.find_first_of("\n\r",pos);
    if (pos != std::string::npos)
        cookieStr = pageStr.substr(pos + 11,(posn - pos) - 11);

    pos = pageStr.find("\r\n\r\n");
    posn = pageStr.find("\r\n",pos + 4);
    if (pos!= std::string::npos) {
        headers = pageStr.substr(0,posn+2);
        buffer.erase(buffer.begin(),buffer.begin() + posn + 2);
        }
    sclose();
	return true;
    }

#else
opensslObj::opensslObj(void *app) :DynamicLibrary("ssl") { 
	throw std::runtime_error("compiled without openSSL, function not available");};
#endif
