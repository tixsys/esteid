/*!
	\file		PCSCManager.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 07:07:55 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 470 $
#pragma once
#include <smartcard++/ManagerInterface.h>
#include <smartcard++/DynamicLibrary.h>
#ifndef _WIN32
#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>
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
define CSTRTYPE LPCSTR
#else
#define CSTRTYPE LPSTR
#endif
#define STRTYPE LPSTR
#ifndef SCARD_E_NO_READERS_AVAILABLE
#define SCARD_E_NO_READERS_AVAILABLE SCARD_E_READER_UNAVAILABLE
#endif
#endif

#ifndef SCARD_CTL_CODE
#ifdef _WIN32
#include <winioctl.h>
#define SCARD_CTL_CODE(code) CTL_CODE(FILE_DEVICE_SMARTCARD,(code),METHOD_BUFFERED,FILE_ANY_ACCESS)
#else
#define SCARD_CTL_CODE(code) (0x42000000 + (code))
#endif
#endif

#define CM_IOCTL_GET_FEATURE_REQUEST SCARD_CTL_CODE(3400)

#define FEATURE_VERIFY_PIN_START         0x01
#define FEATURE_VERIFY_PIN_FINISH        0x02
#define FEATURE_MODIFY_PIN_START         0x03
#define FEATURE_MODIFY_PIN_FINISH        0x04
#define FEATURE_GET_KEY_PRESSED          0x05
#define FEATURE_VERIFY_PIN_DIRECT        0x06
#define FEATURE_MODIFY_PIN_DIRECT        0x07
#define FEATURE_MCT_READERDIRECT         0x08
#define FEATURE_MCT_UNIVERSAL            0x09
#define FEATURE_IFD_PIN_PROPERTIES       0x0A
#define FEATURE_ABORT                    0x0B
#define FEATURE_SET_SPE_MESSAGE          0x0C
#define FEATURE_VERIFY_PIN_DIRECT_APP_ID 0x0D
#define FEATURE_MODIFY_PIN_DIRECT_APP_ID 0x0E
#define FEATURE_WRITE_DISPLAY            0x0F
#define FEATURE_GET_KEY                  0x10
#define FEATURE_IFD_DISPLAY_PROPERTIES   0x11

typedef struct {
        uint16_t wLcdLayout; /**< display characteristics */
        uint16_t wLcdMaxCharacters;
        uint16_t wLcdMaxLines;
        uint8_t bEntryValidationCondition;
        uint8_t bTimeOut2;
} PIN_PROPERTIES_STRUCTURE;

/** the structure must be 6-bytes long */
typedef struct
{
        uint8_t tag;
        uint8_t length;
        uint32_t value; /**< This value is always in BIG ENDIAN format as documented in PCSC v2 part 10 ch 2.2 page 2. You can use ntohl() for example */
} PCSC_TLV_STRUCTURE;


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
	PCSCConnection(ManagerInterface &iface,unsigned int index,bool force) :
		ConnectionBase(iface,index,force) {}
	PCSCConnection(ManagerInterface &iface,SCARDHANDLE existing,DWORD mProto): 
		ConnectionBase(iface),hScard(existing),proto(mProto) {}
	~PCSCConnection() {
	}
	virtual bool isSecure() {return (verify_ioctl || (verify_ioctl_start && verify_ioctl_finish)) && (modify_ioctl || (modify_ioctl_start && modify_ioctl_finish));}
};

/// WinSCard/PCSCLite wrapper
/** PCSCManager represents WinSCard subsystem on Windows or PCSCLite libary
  on platforms where it is available. It loads the libraries dynamically to avoid
 linked-in dependencies */
class PCSCManager : public ManagerInterface {
	DynamicLibrary mLibrary;
	bool mOwnContext;
#ifdef _WIN32
	HANDLE mSCStartedEvent;
	OSVERSIONINFO osVersionInfo;
#endif
	SCARDCONTEXT mSCardContext;
	std::vector<char > mReaders;
	std::vector<SCARD_READERSTATE> mReaderStates;

#ifdef _WIN32
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

	void makeConnection(ConnectionBase *c,uint idx);
	void deleteConnection(ConnectionBase *c);
	void beginTransaction(ConnectionBase *c);
	void endTransaction(ConnectionBase *c,bool forceReset = false);
	void execCommand(ConnectionBase *c,std::vector<BYTE> &cmd,std::vector<BYTE> &recv,
		unsigned int &recvLen);
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
	PCSCConnection * connect(uint idx,bool forceT0);
	/// connect using an application-supplied connection handle
	PCSCConnection * connect(SCARDHANDLE existingHandle);
	PCSCConnection * reconnect(ConnectionBase *c,bool forceT0);
};
