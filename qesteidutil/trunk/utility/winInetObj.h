/*!
	\file		winInetObj.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author$
	\date		$Date$
*/
// Revision $Revision$
#pragma once
#ifdef WIN32
#include "cardlib/DynamicLibrary.h" //bad ..
#include <windows.h>
#include <wininet.h>

/*
	This whole thing serves only one useful purpose at the moment
	downloading HTTPS pages with client auth cert, in function
		inetConnect::getHttpsFile
	Everything else is just a funky chickendance around it to initialize
	wininet, open and close connections and handles and so on.
*/

#define STD __stdcall

class winInetObj : public DynamicLibrary
{
	friend struct inetConnect;
	friend struct inetHttpRequest;
	friend struct inetFtpFileRequest;
protected:
	HINTERNET (STD *pInternetOpenA)(
		IN LPCSTR lpszAgent,IN DWORD dwAccessType,
		IN LPCSTR lpszProxy OPTIONAL,IN LPCSTR lpszProxyBypass OPTIONAL,
		IN DWORD dwFlags);
	BOOL		(STD  *pInternetCloseHandle)(IN HINTERNET hInternet);
	HINTERNET	(STD  *pInternetConnectA)(IN HINTERNET hInternet, IN LPCSTR lpszServerName,
		IN INTERNET_PORT nServerPort, IN LPCSTR lpszUserName OPTIONAL, IN LPCSTR lpszPassword OPTIONAL,
		IN DWORD dwService, IN DWORD dwFlags, IN DWORD_PTR dwContext);
	BOOL (STD  *pInternetReadFile)(
		IN HINTERNET hFile,   IN LPVOID lpBuffer,
		IN DWORD dwNumberOfBytesToRead,OUT LPDWORD lpdwNumberOfBytesRead);
	DWORD (STD  *pInternetErrorDlg)(
		IN HWND hWnd,IN OUT HINTERNET hRequest,
		IN DWORD dwError,IN DWORD dwFlags,
		IN OUT LPVOID * lppvData);
	BOOL (STD  *pHttpSendRequestA)(
		IN HINTERNET hRequest,IN LPCSTR lpszHeaders OPTIONAL,IN DWORD dwHeadersLength,
		IN LPVOID lpOptional OPTIONAL,IN DWORD dwOptionalLength);
	BOOL (STD  *pHttpQueryInfoA)(
		IN HINTERNET hRequest,IN DWORD dwInfoLevel,
		IN OUT LPVOID lpBuffer OPTIONAL,IN OUT LPDWORD lpdwBufferLength,
		IN OUT LPDWORD lpdwIndex OPTIONAL);
	HINTERNET (STD  *pHttpOpenRequestA)(
		IN HINTERNET hConnect,IN LPCSTR lpszVerb,IN LPCSTR lpszObjectName,IN LPCSTR lpszVersion,
		IN LPCSTR lpszReferrer OPTIONAL,IN LPCSTR FAR * lplpszAcceptTypes OPTIONAL,IN DWORD dwFlags,
		IN DWORD_PTR dwContext );
	BOOL (STD  *pInternetSetOptionA)(
		HINTERNET hInternet, DWORD dwOption,
		LPVOID lpBuffer, DWORD dwBufferLength
		);
	HINTERNET (STD *pFtpOpenFileA)(
		IN HINTERNET hConnect,IN LPCSTR lpszFileName,IN DWORD dwAccess,
		IN DWORD dwFlags,IN DWORD_PTR dwContext);
	BOOL (STD *pInternetCrackUrlA)(
		IN LPCSTR lpszUrl,IN DWORD dwUrlLength,
		IN DWORD dwFlags,IN OUT LPURL_COMPONENTSA lpUrlComponents
		);

	HINTERNET hSession;
public:
	winInetObj(void *appPtr);
	void init(int ) {}
	~winInetObj(void);
};

struct inetGenericRequest;
struct inetConnect {
	winInetObj &mNet;
	HINTERNET connect;
	PCCERT_CONTEXT authCert;
	bool authenticated;
	inetConnect(winInetObj &net,std::string site,connType type,void *authCert);
	~inetConnect();
	operator HINTERNET() const {return connect;}
	bool getHttpsFile(std::string url,std::vector<byte> &buffer);
	bool getHttpFile(std::string url,std::vector<byte> &buffer);
	bool getFtpFile(std::string file,std::vector<byte> &buffer);
	static bool getAnyFile(std::string url,std::vector<byte> &buffer);
	bool readFile(std::string name,inetGenericRequest &req,std::vector<byte> &buffer);
	};
#endif
