/*!
	\file		PCSCManager.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 07:07:55 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 470 $
#pragma once
#include "ManagerInterface.h"
#include "DynamicLibrary.h"
#ifndef _WIN32
#ifdef __APPLE__
typedef void *LPVOID;
#define __COREFOUNDATION_CFPLUGINCOM__
#endif
#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>
#include <arpa/inet.h>
#else
#pragma warning(push)
#pragma warning(disable:4201)
#include <winscard.h>
#pragma warning(pop)
#endif

#ifdef _WIN32
#define SCAPI __stdcall
#define CSTRTYPE const CHAR *
#define STRTYPE CHAR *
#undef SCARD_READERSTATE
#define SCARD_READERSTATE SCARD_READERSTATEA
#else
#define SCAPI
#ifdef LPCTSTR
#define CSTRTYPE LPCSTR
#else
#define CSTRTYPE LPSTR
#endif
#define STRTYPE LPSTR
#ifndef SCARD_E_NO_READERS_AVAILABLE
#define SCARD_E_NO_READERS_AVAILABLE SCARD_E_READER_UNAVAILABLE
#endif
#endif

/// Holds connection parameters for PCSC card connection
struct PCSCConnection : public ConnectionBase {
	SCARDHANDLE hScard;
	DWORD proto;
	DWORD verify_ioctl;
	DWORD verify_ioctl_start;
	DWORD verify_ioctl_finish;
	DWORD modify_ioctl;
	DWORD modify_ioctl_start;
	DWORD modify_ioctl_finish;
	bool display;
	bool pinpad;
	PCSCConnection(ManagerInterface &iface,unsigned int index) :
		ConnectionBase(iface,index) {}
	PCSCConnection(ManagerInterface &iface, SCARDHANDLE existing, DWORD mProto): 
		ConnectionBase(iface),hScard(existing),proto(mProto) {}
	~PCSCConnection() {}
	virtual bool isSecure() {return pinpad;}
};

/// WinSCard/PCSCLite wrapper
/** PCSCManager represents WinSCard subsystem on Windows or PCSCLite libary
  on platforms where it is available. It loads the libraries dynamically to avoid
 linked-in dependencies */
class PCSCManager : public ManagerInterface {
private:
	DynamicLibrary mLibrary;
	bool mOwnContext;
	SCARDCONTEXT mSCardContext;
	std::vector<char > mReaders;
	std::vector<SCARD_READERSTATE> mReaderStates;

#ifdef _WIN32
	HANDLE mSCStartedEvent;
	HANDLE (SCAPI *pSCardAccessStartedEvent)();
	void (SCAPI *pSCardReleaseStartedEvent)(HANDLE hStartedEventHandle);
#endif
	LONG (SCAPI *pSCardEstablishContext)(DWORD scope,LPCVOID res1,LPCVOID res2,SCARDCONTEXT *context);
	LONG (SCAPI *pSCardReleaseContext)(SCARDCONTEXT hContext);
	LONG (SCAPI *pSCardGetStatusChange)(SCARDCONTEXT hContext,DWORD dwTimeout,SCARD_READERSTATE *rgReaderStates,DWORD cReaders);
	LONG (SCAPI *pSCardListReaders)(SCARDCONTEXT hContext,CSTRTYPE mszGroups,STRTYPE mszReaders,LPDWORD pcchReaders);
	LONG (SCAPI *pSCardTransmit)(SCARDHANDLE hCard,LPCSCARD_IO_REQUEST pioSendPci,
		LPCBYTE pbSendBuffer,DWORD cbSendLength,
		LPSCARD_IO_REQUEST pioRecvPci,LPBYTE pbRecvBuffer,
		LPDWORD pcbRecvLength);
	LONG (SCAPI *pSCardGetAttrib)(SCARDHANDLE hCard,DWORD dwAttrId,LPBYTE pbAttr,LPDWORD pcbAttrLen);
	LONG (SCAPI *pSCardConnect)(SCARDCONTEXT hContext,CSTRTYPE szReader,DWORD dwShareMode,
		DWORD dwPreferredProtocols,LPSCARDHANDLE phCard,LPDWORD pdwActiveProtocol);
	LONG (SCAPI *pSCardReconnect)(SCARDHANDLE hCard, DWORD dwShareMode,DWORD dwPreferredProtocols,DWORD dwInitialization,
		LPDWORD pdwActiveProtocol);
	LONG (SCAPI *pSCardDisconnect)(SCARDHANDLE hCard,DWORD dwDisposition);
	LONG (SCAPI *pSCardBeginTransaction)(SCARDHANDLE hCard);
	LONG (SCAPI *pSCardEndTransaction)(	SCARDHANDLE hCard,DWORD dwDisposition);
	LONG (SCAPI *pSCardStatus)( SCARDHANDLE hCard,STRTYPE szReaderName,LPDWORD pcchReaderLen,
		LPDWORD pdwState,LPDWORD pdwProtocol,LPBYTE pbAtr,LPDWORD pcbAtrLen);
	LONG (SCAPI *pSCardControl)(SCARDHANDLE hCard, DWORD dwControlCode, LPCVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);

	void construct(void);
	void ensureReaders(uint idx);

	void execPinCommand(ConnectionBase *c, bool verify, std::vector<byte> &cmd);

protected:
	void makeConnection(ConnectionBase *c, uint idx);
	void deleteConnection(ConnectionBase *c);
	void beginTransaction(ConnectionBase *c);
	void endTransaction(ConnectionBase *c,bool forceReset = false);
	void execCommand(ConnectionBase *c,std::vector<BYTE> &cmd,std::vector<BYTE> &recv,
		unsigned int &recvLen);
	void execPinEntryCommand(ConnectionBase *c,std::vector<byte> &cmd);
	void execPinChangeCommand(ConnectionBase *c,std::vector<byte> &cmd, size_t oldPinLen, size_t newPinLen);
	bool isT1Protocol(ConnectionBase *c);

public:
	PCSCManager(void);
	/// construct with application-supplied card context
	PCSCManager(SCARDCONTEXT existingContext);
	~PCSCManager(void);
	uint getReaderCount(bool forceRefresh = false);
	std::string getReaderName(uint idx);
	std::string getReaderState(uint idx);
	std::string getATRHex(uint idx);
	void getATRHex(ConnectionBase* conn, BYTE* buf, DWORD* buf_size);
	PCSCConnection * connect(uint idx);
	/// connect using an application-supplied connection handle
	PCSCConnection * connect(SCARDHANDLE existingHandle);
	PCSCConnection * reconnect(ConnectionBase *c);
};
