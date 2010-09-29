/*!
	\file		PCSCManager.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 07:07:55 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 470 $

#ifdef _WIN32
// This must be included first because other crap may pull in the conflicting ws2def.h
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <smartcardpp/PCSCManager.h>
#include <smartcardpp/SCError.h>
#include <smartcardpp/CardBase.h> //for exceptions
#include "common.h"

#ifndef CM_IOCTL_GET_FEATURE_REQUEST

// FIXME: We should use internal-winscard.h from OpenSC project instead
//        This will clean up PCSCManager.h header considerably and
//        allows us to use Mingw32 for building

#include "internal-pcsc22.h"
#endif

#ifdef _WIN32
#define LIBNAME "winscard"
#define SUFFIX "A"
#else
#if defined(__APPLE__)
#define LIBNAME "PCSC.framework/PCSC"
#define SUFFIX ""
#else
#define LIBNAME "pcsclite"
#define SUFFIX ""
#endif
#endif

using std::string;

PCSCManager::PCSCManager(void): mLibrary(LIBNAME),mOwnContext(true) 
{
	construct();
	SCError::checkError((*pSCardEstablishContext)(SCARD_SCOPE_USER,
					NULL,
					NULL,
					&mSCardContext));
}

PCSCManager::PCSCManager(SCARDCONTEXT existingContext): mLibrary(LIBNAME),mOwnContext(false) 
{
	construct();
	mSCardContext=existingContext;
}

void PCSCManager::construct()
{
#ifdef _WIN32
	osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
	GetVersionEx(&osVersionInfo);
	pSCardAccessStartedEvent = ( HANDLE(SCAPI *)() )
		mLibrary.getProc("SCardAccessStartedEvent");
	pSCardReleaseStartedEvent = (void(SCAPI *)(HANDLE))mLibrary.getProc("SCardReleaseStartedEvent");
#endif
	pSCardEstablishContext = (LONG(SCAPI *)(DWORD,LPCVOID,LPCVOID,SCARDCONTEXT*))
		mLibrary.getProc("SCardEstablishContext");
	pSCardReleaseContext = (LONG(SCAPI *)(SCARDCONTEXT))
		mLibrary.getProc("SCardReleaseContext");
	pSCardGetStatusChange = (LONG(SCAPI *)(SCARDCONTEXT,DWORD,SCARD_READERSTATE*,DWORD))
		mLibrary.getProc("SCardGetStatusChange" SUFFIX);
	pSCardListReaders = (LONG(SCAPI *)(SCARDCONTEXT,CSTRTYPE,STRTYPE,LPDWORD))
		mLibrary.getProc("SCardListReaders"  SUFFIX);
	pSCardTransmit = (LONG(SCAPI *)(SCARDHANDLE,LPCSCARD_IO_REQUEST,LPCBYTE,DWORD,LPSCARD_IO_REQUEST,LPBYTE,LPDWORD))
		mLibrary.getProc("SCardTransmit");
	try {
		pSCardGetAttrib = (LONG(SCAPI *)(SCARDHANDLE,DWORD,LPBYTE,LPDWORD))
			mLibrary.getProc("SCardGetAttrib");
	} catch(std::runtime_error) {
		pSCardGetAttrib = NULL; // proc not found, Old PC/SC API
	}
	pSCardConnect = (LONG(SCAPI *)(SCARDCONTEXT ,CSTRTYPE ,DWORD ,DWORD ,SCARDHANDLE *,LPDWORD ))
		mLibrary.getProc("SCardConnect"  SUFFIX);
	pSCardReconnect = (LONG(SCAPI *)(SCARDHANDLE , DWORD ,DWORD ,DWORD ,LPDWORD ))
		mLibrary.getProc("SCardReconnect");
	pSCardDisconnect = (LONG (SCAPI *)(SCARDHANDLE hCard,DWORD dwDisposition))
		mLibrary.getProc("SCardDisconnect");
	pSCardBeginTransaction = (LONG(SCAPI *)(SCARDHANDLE ))
		mLibrary.getProc("SCardBeginTransaction");
	pSCardEndTransaction=(LONG(SCAPI *)(SCARDHANDLE ,DWORD ))
		mLibrary.getProc("SCardEndTransaction");

	if(pSCardGetAttrib)
#ifdef __APPLE__
		pSCardControl=(LONG (SCAPI *)(SCARDHANDLE, DWORD, LPCVOID, DWORD, LPVOID, DWORD, LPDWORD))
			mLibrary.getProc("SCardControl132");
#else
		pSCardControl=(LONG (SCAPI *)(SCARDHANDLE, DWORD, LPCVOID, DWORD, LPVOID, DWORD, LPDWORD))
			mLibrary.getProc("SCardControl");
#endif
	else
		// Old API no usable pSCardControl
		pSCardControl = NULL;
	
#ifdef _WIN32
	pSCardStatus = (LONG(SCAPI *)(SCARDHANDLE ,STRTYPE ,LPDWORD ,
		LPDWORD ,LPDWORD ,LPBYTE ,LPDWORD ))
		mLibrary.getProc("SCardStatus" SUFFIX);

	if (!((osVersionInfo.dwMajorVersion == 5) && (osVersionInfo.dwMinorVersion == 0) )) { //win2K
		mSCStartedEvent = (*pSCardAccessStartedEvent)();
		if (!mSCStartedEvent) {
			LPVOID lpMsgBuf;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
							NULL, GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) &lpMsgBuf,	0, NULL );
			std::ostringstream buf;
			buf << "SCardAccessStartedEvent returns NULL\n" << (LPCTSTR)lpMsgBuf;
			throw std::runtime_error(buf.str());
			}
		//the timeout here is NEEDED under Vista/Longhorn, do not remove it
		if (WAIT_OBJECT_0 != WaitForSingleObject(mSCStartedEvent,1000) ) {
			throw std::runtime_error("Smartcard subsystem not started");
			}
		}
#endif
}

PCSCManager::~PCSCManager(void)
{
	if (mOwnContext)
		(*pSCardReleaseContext)(mSCardContext);
#ifdef _WIN32
// this crashes with "ESP not being preserved", wrong calling convention apparently
//	(*pSCardReleaseStartedEvent)(mSCStartedEvent);
#endif
}

void PCSCManager::ensureReaders(uint idx)
{
	DWORD ccReaders = 0;
	SCError::checkError((*pSCardListReaders)(mSCardContext,NULL,NULL,&ccReaders));
	if (ccReaders == 0) {
		mReaderStates.clear();
		return;
		}
	if (ccReaders != mReaders.size()) { //check whether we have listed already
		mReaderStates.clear();
		mReaders.resize(ccReaders);
		SCError::checkError((*pSCardListReaders)(mSCardContext,NULL,&mReaders[0],&ccReaders));
		char* p = &mReaders[0];
		while(p < &*(--mReaders.end()) ) {
			SCARD_READERSTATE s = {p,NULL,SCARD_STATE_UNAWARE,0,0,{'\0'}};
			mReaderStates.push_back(s);
			p+= string(p).length() + 1;
			}
		if (mReaderStates.size() ==  0 )
			throw SCError(SCARD_E_READER_UNAVAILABLE);
		}
	
#ifdef __APPLE__
	for(size_t i = 0; i < mReaderStates.size(); i++) {
		SCError::checkError((*pSCardGetStatusChange)(mSCardContext, 0, &mReaderStates[i], 1));
	}
#else
	SCError::checkError((*pSCardGetStatusChange)(mSCardContext,0, &mReaderStates[0],DWORD(mReaderStates.size())));
#endif
	if (idx >= mReaderStates.size())
		throw std::range_error("ensureReaders: Index out of bounds");
}

uint PCSCManager::getReaderCount(bool forceRefresh)
{
	if (forceRefresh) 
		mReaders.clear();
	try {
		ensureReaders(0);
	} catch(SCError &err) {
		if (err.error == long(SCARD_E_NO_READERS_AVAILABLE))
			throw SCError(SCARD_E_READER_UNAVAILABLE);
		else
			throw err;
		}
	return (uint) mReaderStates.size();
}

string PCSCManager::getReaderName(uint idx)
{
	ensureReaders(idx);
	return mReaderStates[idx].szReader;
}

#define SS(a) if ((theState & SCARD_STATE_##a ) == SCARD_STATE_##a) \
	stateStr += string(#a) + string("|")

string PCSCManager::getReaderState(uint idx)
{
	ensureReaders(idx);
	DWORD theState = mReaderStates[idx].dwEventState;
	string stateStr = "";
	SS(IGNORE);
	SS(UNKNOWN);
	SS(UNAVAILABLE);
	SS(EMPTY);
	SS(PRESENT);
	SS(ATRMATCH);
	SS(EXCLUSIVE);
	SS(INUSE);
	SS(MUTE);
#ifdef SCARD_STATE_UNPOWERED
	SS(UNPOWERED);
#endif
	if (stateStr.length() > 0 ) stateStr = stateStr.substr(0,stateStr.length()-1);
	return stateStr ;
}

string PCSCManager::getATRHex(uint idx)
{
	ensureReaders(idx);
	std::ostringstream buf;
	buf << "";
	for(uint i=0;i<mReaderStates[idx].cbAtr;i++)
		buf << std::setfill('0') << std::setw(2) <<std::hex <<
		(short) mReaderStates[idx].rgbAtr[i];
	string retval = buf.str();
	return retval;
}

PCSCConnection * PCSCManager::connect(uint idx,bool forceT0)
{
	ensureReaders(idx);
	return new PCSCConnection(*this,idx,forceT0);
}

PCSCConnection * PCSCManager::connect(SCARDHANDLE existingHandle) {
	DWORD proto = SCARD_PROTOCOL_T0;
#ifdef _WIN32 //quick hack, pcsclite headers dont have that
	DWORD tmpProto,sz=sizeof(DWORD);
	if (!(*pSCardGetAttrib)(existingHandle,SCARD_ATTR_CURRENT_PROTOCOL_TYPE,
		(LPBYTE)&tmpProto,&sz)) 
		proto = tmpProto;
#endif
	return new PCSCConnection(*this,existingHandle,proto);
}

PCSCConnection * PCSCManager::reconnect(ConnectionBase *c,bool forceT0) {
	UNUSED_ARG(forceT0); //??
	PCSCConnection *pc = (PCSCConnection *)c;
	SCError::checkError((*pSCardReconnect)(pc->hScard, 
		SCARD_SHARE_SHARED, (pc->mForceT0 ? 0 : SCARD_PROTOCOL_T1 ) | SCARD_PROTOCOL_T0,
		SCARD_RESET_CARD,&pc->proto));
	return pc;
	}

void PCSCManager::makeConnection(ConnectionBase *c,uint idx)
{
	PCSCConnection *pc = (PCSCConnection *)c;
	DWORD i, feature_len, rcount;
	PCSC_TLV_STRUCTURE *pcsc_tlv;
	BYTE feature_buf[256], rbuf[256];
	LONG rv;
	
	SCError::checkError((*pSCardConnect)(mSCardContext, (CSTRTYPE) mReaderStates[idx].szReader,
		SCARD_SHARE_SHARED,
		(pc->mForceT0 ? 0 : SCARD_PROTOCOL_T1 ) | SCARD_PROTOCOL_T0
		, & pc->hScard,& pc->proto));

	// Is there a pinpad?
	pc->verify_ioctl = pc->verify_ioctl_start = pc->verify_ioctl_finish = 0;
	pc->modify_ioctl = pc->modify_ioctl_start = pc->modify_ioctl_finish = 0;
	pc->pinpad = pc->display = false;

	if(!pSCardControl) return; // PC/SC API is too old to support PinPADs

	rv = (*pSCardControl)(pc->hScard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0, feature_buf, sizeof(feature_buf), &feature_len);
	if (rv == SCARD_S_SUCCESS) {
		if ((feature_len % sizeof(PCSC_TLV_STRUCTURE)) == 0) {
			feature_len /= sizeof(PCSC_TLV_STRUCTURE);
			pcsc_tlv = (PCSC_TLV_STRUCTURE *)feature_buf;
			for (i = 0; i < feature_len; i++) {
				if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_DIRECT) {
					pc->verify_ioctl = ntohl(pcsc_tlv[i].value);
				} else if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_START) {
					pc->verify_ioctl_start = ntohl(pcsc_tlv[i].value);
				} else if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_FINISH) {
					pc->verify_ioctl_finish = ntohl(pcsc_tlv[i].value);
				} else if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_DIRECT) {
					pc->modify_ioctl = ntohl(pcsc_tlv[i].value);
				} else if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_START) {
					pc->modify_ioctl_start = ntohl(pcsc_tlv[i].value);
				} else if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_FINISH) {
					pc->modify_ioctl_finish = ntohl(pcsc_tlv[i].value);
				} else if (pcsc_tlv[i].tag == FEATURE_IFD_PIN_PROPERTIES) {
					if ((*pSCardControl)(pc->hScard,  ntohl(pcsc_tlv[i].value), NULL, 0, rbuf, sizeof(rbuf), &rcount) == SCARD_S_SUCCESS) {
						PIN_PROPERTIES_STRUCTURE *caps = (PIN_PROPERTIES_STRUCTURE *)rbuf;
						if (caps->wLcdLayout > 0)
							pc->display=true; 
					}
				}	
			}
		}
	}

	if((pc->verify_ioctl || (pc->verify_ioctl_start && pc->verify_ioctl_finish)) &&
	   (pc->modify_ioctl || (pc->modify_ioctl_start && pc->modify_ioctl_finish)))
		pc->pinpad = true;

	/* Force T=0 for PinPad readers. T=1 is terminally broken. */
	if(pc->proto != SCARD_PROTOCOL_T0 && pc->pinpad) {
		pc->mForceT0 = true;
		deleteConnection(c);

		SCError::checkError((*pSCardConnect)(mSCardContext,
				(CSTRTYPE) mReaderStates[idx].szReader,
				SCARD_SHARE_SHARED,
				SCARD_PROTOCOL_T0,
				& pc->hScard,& pc->proto));
	}
}

void PCSCManager::deleteConnection(ConnectionBase *c)
{
	SCError::checkError((*pSCardDisconnect)((( PCSCConnection *)c)->hScard,SCARD_RESET_CARD));
}

void PCSCManager::beginTransaction(ConnectionBase *c)
{
	SCError::checkError((*pSCardBeginTransaction)( (( PCSCConnection *)c)->hScard));
}

void PCSCManager::endTransaction(ConnectionBase *c,bool forceReset)
{
	if (forceReset) { //workaround for reader driver bug
		BYTE _rdrBuf[1024];
		STRTYPE reader = (STRTYPE) _rdrBuf;
		DWORD rdrLen = sizeof(reader);
		DWORD state,proto,result,active;
		BYTE atr[1024];
		DWORD atrLen = sizeof(atr);
		result = (*pSCardStatus)((( PCSCConnection *)c)->hScard,reader,&rdrLen,&state,&proto,atr,&atrLen);
		if (result == SCARD_W_RESET_CARD) {
			// FIXME: This will allow re-connecting with T1 even when T0 is forced
			result = (*pSCardReconnect)((( PCSCConnection *)c)->hScard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, 
				SCARD_LEAVE_CARD,&active);
			(*pSCardStatus)((( PCSCConnection *)c)->hScard,reader,&rdrLen,&state,&proto,atr,&atrLen);
			}
		}
	/*SCError::checkError(*/
	(*pSCardEndTransaction)((( PCSCConnection *)c)->hScard,forceReset ? SCARD_RESET_CARD : SCARD_LEAVE_CARD)
		/*)*/;
}

void PCSCManager::execCommand(ConnectionBase *c,std::vector<BYTE> &cmd
		,std::vector<BYTE> &recv,
		uint &recvLen) {
	PCSCConnection *pc = (PCSCConnection *)c;
	const SCARD_IO_REQUEST _MT0 = {1,8};
	const SCARD_IO_REQUEST _MT1 = {2,8};

	DWORD ret = recvLen;
	SCError::checkError((*pSCardTransmit)(pc->hScard,
				pc->proto == SCARD_PROTOCOL_T0 ? &_MT0 : &_MT1 ,
				&cmd[0],(DWORD)cmd.size() ,
				NULL,
				&recv[0] , &ret));
	recvLen = (uint)(ret);
}

#define SET_ESTEID_PINFORMAT(a) \
	a->bTimerOut = 30; \
	a->bTimerOut2 = 30; \
	a->bmFormatString = 0x02; /* Ascii */ \
	a->bmPINBlockString = 0x00; \
	a->bmPINLengthFormat = 0x00; \
	a->bEntryValidationCondition = 0x02; /* Keypress only */ \
	/* FIXME: Min/Max pin length should be passed to us from CardBase */ \
	a->wPINMaxExtraDigit = (4 << 8 ) + 12; /* little endian! */ \
	/* Ignore language and T=1 parameters. */ \
	a->wLangId = 0x0000; \
	a->bTeoPrologue[0] = 0x00; \
	a->bTeoPrologue[1] = 0x00; \
	a->bTeoPrologue[2] = 0x00;


void PCSCManager::execPinCommand(ConnectionBase *c, bool verify, std::vector<byte> &cmd) {
	if(!pSCardControl)
		throw std::runtime_error("PC/SC API is too old");

	PCSCConnection *pc = (PCSCConnection *)c;

	/* Force T=0 for PinPad commands. T=1 is terminally broken.
         * Actually the same check is done in makeConnection
         * so this is redundant.
         */
	if(pc->proto != SCARD_PROTOCOL_T0) {
		pc->mForceT0 = true;
		reconnect(pc, true);
	}
	
	BYTE sbuf[256], rbuf[256];
	DWORD ioctl = 0, ioctl2 = 0, rlen=sizeof(rbuf), count = 0;
	size_t offset = 0;
	
	// build PC/SC block. FIXME: hardcoded for EstEID!!!
	if (verify) {
		PIN_VERIFY_STRUCTURE *pin_verify  = (PIN_VERIFY_STRUCTURE *)sbuf;
		SET_ESTEID_PINFORMAT(pin_verify);

		if(pc->verify_ioctl_start) {
			ioctl = pc->verify_ioctl_start;
			ioctl2 = pc->verify_ioctl_finish;
		} else {
			ioctl = pc->verify_ioctl;
		}

		/* Show default message on display */
		pin_verify->bNumberMessage = pc->display ? 0xFF: 0x00;
		pin_verify->bMsgIndex = 0x00;

		/* Set proper command sizes */
		pin_verify->ulDataLength = (uint32_t)cmd.size();
		count = sizeof(PIN_VERIFY_STRUCTURE) +
			pin_verify->ulDataLength - 1;
		offset = (byte *)(pin_verify->abData) - (byte *)pin_verify;

	} else {
		PIN_MODIFY_STRUCTURE *pin_modify  = (PIN_MODIFY_STRUCTURE *)sbuf;
		SET_ESTEID_PINFORMAT(pin_modify);

		if(pc->modify_ioctl_start) {
			ioctl = pc->modify_ioctl_start;
			ioctl2 = pc->modify_ioctl_finish;
		} else {
			ioctl = pc->modify_ioctl;
		}

		pin_modify->bInsertionOffsetOld = 0x00;
		pin_modify->bInsertionOffsetNew = 0x00;
		pin_modify->bConfirmPIN = 0x03;

		/* Default messages */
		pin_modify->bNumberMessage = pc->display ? 0x03 : 0x00;
		pin_modify->bMsgIndex1 = 0x00;
		pin_modify->bMsgIndex2 = 0x01;
		pin_modify->bMsgIndex3 = 0x02;

		/* Set proper command sizes */
		pin_modify->ulDataLength = (uint32_t)cmd.size();
		count = sizeof(PIN_MODIFY_STRUCTURE) +
			pin_modify->ulDataLength - 1;
		offset = (byte *)pin_modify->abData - (byte *)pin_modify;
	}

	/* Copy APDU itself */
	for (size_t i = 0; i < cmd.size(); i++)
		sbuf[offset + i] = cmd[i];

	SCError::checkError((*pSCardControl)(pc->hScard, ioctl, sbuf, count,
					rbuf, sizeof(rbuf), &rlen));

	// finish a two phase operation
	if (ioctl2) {
		rlen = sizeof(rbuf);
		SCError::checkError((*pSCardControl)(pc->hScard, ioctl2, NULL, 0, rbuf, sizeof(rbuf), &rlen));
	}
	byte SW1 = rbuf[rlen - 2];
	byte SW2 = rbuf[rlen - 1];
	if (SW1 != 0x90) {
                // 0x00 - Timeout (SCM)
                // 0x01 - Cancel pressed (OK, SCM)
                // 0x03 - Omnikey sends this when Pin was too short
		if (SW1==0x64 && ( SW2 == 0x00 || SW2 == 0x01 || SW2 == 0x02) ) {
			AuthError err(SW1,SW2);
			err.m_aborted = true;
			throw err;
		}
		throw CardError(SW1, SW2);
	}
}

void PCSCManager::execPinEntryCommand(ConnectionBase *c,std::vector<byte> &cmd) {
	execPinCommand(c, true, cmd);
}

void PCSCManager::execPinChangeCommand(ConnectionBase *c,std::vector<byte> &cmd, size_t oldPinLen, size_t newPinLen) {
	execPinCommand(c, false, cmd);
}
         


bool PCSCManager::isT1Protocol(ConnectionBase *c) {
	PCSCConnection *pc = (PCSCConnection *)c;
	return pc->proto == SCARD_PROTOCOL_T1 && !pc->mForceT0;
	}
