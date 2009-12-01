/*!
	\file		winInetObj.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-06-18 12:56:43 +0300 (Thu, 18 Jun 2009) $
*/
// Revision $Revision: 329 $

#include "precompiled.h"
#include "netObj.h"
#include "winInetObj.h"

#ifdef _WIN32

using std::string;
using std::vector;

struct inetGenericRequest {
	virtual operator HINTERNET() const = 0;
	};

inetError::inetError(string op) : 
	error(GetLastError()), std::runtime_error("WinInet error") {
	std::ostringstream buf;
	buf << "WinInet exception:'" << op << 
		"' code:'0x" <<std::hex << std::setfill('0') <<
		std::setw(8) << error << "'";
	desc = buf.str();
	}
void inetError::check(string op,HINTERNET handle) {
	if (handle == NULL) 
		throw inetError(op);
	}
void inetError::check(string op,BOOL result) {
	if (result == FALSE) 
		throw inetError(op);
	}
bool inetError::isInvalidAuth() {
	return error == ERROR_INTERNET_SECURITY_CHANNEL_ERROR;
	}

winInetObj::winInetObj(void *) : DynamicLibrary("wininet") {
	pInternetOpenA = (HINTERNET (STD *)(
			IN LPCSTR lpszAgent,IN DWORD dwAccessType,
			IN LPCSTR lpszProxy OPTIONAL,IN LPCSTR lpszProxyBypass OPTIONAL,
			IN DWORD dwFlags))
		getProc("InternetOpenA");
	pInternetCloseHandle = (BOOL(STD *)(IN HINTERNET hInternet))
		getProc("InternetCloseHandle");
	pInternetConnectA = (HINTERNET (STD *)(
			IN HINTERNET hInternet, IN LPCSTR lpszServerName,
			IN INTERNET_PORT nServerPort, IN LPCSTR lpszUserName OPTIONAL, 
			IN LPCSTR lpszPassword OPTIONAL,
			IN DWORD dwService, IN DWORD dwFlags, IN DWORD_PTR dwContext))
		getProc("InternetConnectA");
	pInternetReadFile = (BOOL (STD *)(
			IN HINTERNET hFile,   IN LPVOID lpBuffer,
			IN DWORD dwNumberOfBytesToRead,OUT LPDWORD lpdwNumberOfBytesRead))
		getProc("InternetReadFile");
	pInternetErrorDlg = (DWORD (STD *)(
			IN HWND hWnd,IN OUT HINTERNET hRequest,
			IN DWORD dwError,IN DWORD dwFlags,
			IN OUT LPVOID * lppvData))
		getProc("InternetErrorDlg");
	pHttpSendRequestA = (BOOL (STD *)(
			IN HINTERNET hRequest,IN LPCSTR lpszHeaders OPTIONAL,IN DWORD dwHeadersLength,
			IN LPVOID lpOptional OPTIONAL,IN DWORD dwOptionalLength))
		getProc("HttpSendRequestA");
	pHttpQueryInfoA = (BOOL (STD *)(
			IN HINTERNET hRequest,IN DWORD dwInfoLevel,
			IN OUT LPVOID lpBuffer OPTIONAL,IN OUT LPDWORD lpdwBufferLength,
			IN OUT LPDWORD lpdwIndex OPTIONAL))
		getProc("HttpQueryInfoA");
	pHttpOpenRequestA = (HINTERNET (STD *)(
			IN HINTERNET hConnect,IN LPCSTR lpszVerb,IN LPCSTR lpszObjectName,IN LPCSTR lpszVersion,
			IN LPCSTR lpszReferrer OPTIONAL,IN LPCSTR FAR * lplpszAcceptTypes OPTIONAL,IN DWORD dwFlags,
			IN DWORD_PTR dwContext ))
		getProc("HttpOpenRequestA");
	pInternetSetOptionA = (BOOL (STD *)(
			HINTERNET hInternet, DWORD dwOption, 
			LPVOID lpBuffer, DWORD dwBufferLength))
		getProc("InternetSetOptionA");

	pFtpOpenFileA = (HINTERNET (STD *)(
		IN HINTERNET hConnect,IN LPCSTR lpszFileName,IN DWORD dwAccess,
		IN DWORD dwFlags,IN DWORD_PTR dwContext))
		getProc("FtpOpenFileA");
	pInternetCrackUrlA = (BOOL (STD *)(
		IN LPCSTR lpszUrl,IN DWORD dwUrlLength,
		IN DWORD dwFlags,IN OUT LPURL_COMPONENTSA lpUrlComponents
		)) 
		getProc("InternetCrackUrlA");

	inetError::check("openSession",hSession = pInternetOpenA(
                        "Wininet Client App",
                        PRE_CONFIG_INTERNET_ACCESS,
                        NULL, 
                        INTERNET_INVALID_PORT_NUMBER,
                        0));
	}

winInetObj::~winInetObj() {
	pInternetCloseHandle(hSession);
}

inetConnect::inetConnect(winInetObj &net,string site,connType type,void *auth) :
		mNet(net),authCert((PCCERT_CONTEXT)auth),authenticated(false) {
	int port;
	DWORD service;
	if (type == HTTP) {
		port = INTERNET_DEFAULT_HTTP_PORT;
		service = INTERNET_SERVICE_HTTP;
	} else if(type==FTP) {
		port = INTERNET_DEFAULT_FTP_PORT;
		service = INTERNET_SERVICE_FTP;
	} else {
		port = INTERNET_DEFAULT_HTTPS_PORT;
		service = INTERNET_SERVICE_HTTP;
		}
	inetError::check(string("InternetConnect '") + site + "'",
		connect = mNet.pInternetConnectA(
							net.hSession,
							site.c_str(),
							port,"","",
							service,0,0));
	}

inetConnect::~inetConnect() {
		mNet.pInternetCloseHandle(connect);
		}

#define SECUREFLAGS INTERNET_FLAG_SECURE | \
					INTERNET_FLAG_IGNORE_CERT_CN_INVALID | \
                    INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | \
					INTERNET_FLAG_KEEP_CONNECTION

struct inetHttpRequest : public inetGenericRequest {
	HINTERNET file;
	inetConnect &mConnect;
	inetHttpRequest(inetConnect &conn,
			const char *verb,string url,DWORD flags) : mConnect(conn) {
		inetError::check(string("OpenRequest '") + verb + " " + url + "'",
			file = mConnect.mNet.pHttpOpenRequestA(mConnect,
				verb,url.c_str(),NULL,"",NULL,flags,0));
		}
	~inetHttpRequest() {
		mConnect.mNet.pInternetCloseHandle(file);
		}
	operator HINTERNET() const {return file;}
private:
	const inetHttpRequest &operator=(const inetHttpRequest &o);
	};

#define DIALOGOPT FLAGS_ERROR_UI_FILTER_FOR_ERRORS | \
				FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | \
				FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS

bool inetConnect::readFile(std::string name,
						   inetGenericRequest &req, vector<byte> &buffer) {
	DWORD bytesTotal = 0;
    DWORD bytesRead;
	DWORD readChunk = 4096;

	buffer.resize(readChunk * 4);
	do {
		if (bytesTotal > (buffer.size() - readChunk))
			buffer.resize(buffer.size() + (readChunk * 4));
		inetError::check(string("pInternetReadFile '") + name +"'",
			mNet.pInternetReadFile(req, 
			&buffer[bytesTotal],readChunk ,&bytesRead));
		bytesTotal +=bytesRead;
	} while(bytesRead != 0 );
	buffer[bytesTotal] = 0;
	buffer.erase(buffer.begin() + bytesTotal,buffer.end());
	return true;
	}

bool inetConnect::getHttpsFile(
	  string url,vector<byte> &buffer) {
	inetHttpRequest req(*this,"GET",url,SECUREFLAGS);

	if (!authenticated) 
		inetError::check("InternetSetOption(authCert)",
		mNet.pInternetSetOptionA(req,INTERNET_OPTION_CLIENT_CERT_CONTEXT,
			(void *) authCert,sizeof(CERT_CONTEXT)));

	//authenticated = true;
	while(! mNet.pHttpSendRequestA(req,NULL,0,0,0)) {
		if (GetLastError() == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED) {
			mNet.pInternetErrorDlg(NULL,req,
				ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED,DIALOGOPT,NULL);
			}
		else 
			throw inetError(string("SendRequest '") + url +"'");
		}

	DWORD dwCode,dwSize = sizeof(DWORD);
	inetError::check(string("HttpQueryInfo '") + url +"'",
		mNet.pHttpQueryInfoA(req,HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
			&dwCode,&dwSize,NULL));
	if (dwCode != 200 ) {
		std::ostringstream buf;
		buf << url << " not found:" << dwCode;
		throw std::runtime_error(buf.str());
		}

	readFile(url,req,buffer);
	return true;
	}

bool inetConnect::getHttpFile(std::string url,std::vector<byte> &buffer) {
	inetHttpRequest req(*this,"GET",url,INTERNET_FLAG_KEEP_CONNECTION);

	//not implemented/tested, never needed it
	readFile(url,req,buffer);
	return false;
	}

struct inetFtpFileRequest : public inetGenericRequest{
	HINTERNET file;
	inetConnect &mConnect;
	inetFtpFileRequest(inetConnect &conn,string fileName) : mConnect(conn) {
		inetError::check(string("pFtpOpenFile '") + fileName + "'",
		file = mConnect.mNet.pFtpOpenFileA(mConnect,fileName.c_str(),GENERIC_READ,
				FTP_TRANSFER_TYPE_BINARY,0));
		}
	~inetFtpFileRequest() {
		mConnect.mNet.pInternetCloseHandle(file);
		}
	operator HINTERNET() const {return file;}
private:
	const inetFtpFileRequest &operator=(const inetFtpFileRequest &o);
	};

bool inetConnect::getFtpFile(std::string file,std::vector<byte> &buffer) {
	inetFtpFileRequest req(*this,file);

	readFile(file,req,buffer);

	return true;
	}

bool inetConnect::getAnyFile(std::string url,std::vector<byte> &buffer) {
	winInetObj net(NULL);
	net.init(0);

	URL_COMPONENTSA URLparts;
	memset(&URLparts,0,sizeof(URLparts));
	URLparts.dwStructSize = sizeof( URLparts );
	URLparts.dwSchemeLength    = 
	URLparts.dwHostNameLength  = 
	URLparts.dwUserNameLength  = 
	URLparts.dwPasswordLength  = 
	URLparts.dwUrlPathLength   = 
	URLparts.dwExtraInfoLength = 1;

	net.pInternetCrackUrlA(url.c_str(),0,0,&URLparts);
	std::string scheme(URLparts.lpszScheme,URLparts.dwSchemeLength);
	std::string server(URLparts.lpszHostName,URLparts.dwHostNameLength);
	std::string target(URLparts.lpszUrlPath, URLparts.dwUrlPathLength);
	if (scheme == "ftp") {
		netConnect conn(net,server, FTP , 0);
		return conn.getFtpFile(target,buffer);
	} else if(scheme == "https") {
		netConnect conn(net,server, HTTPS ,0 );
		return conn.getHttpsFile(target,buffer);
	} else if(scheme == "http") {
		netConnect conn(net,server, HTTPS, 0);
		return conn.getHttpFile(target,buffer);
		}

	return true;
	}

#endif //_WIN32
