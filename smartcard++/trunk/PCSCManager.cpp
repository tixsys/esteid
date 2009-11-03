/*!
	\file		PCSCManager.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 07:07:55 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 470 $
#include "precompiled.h"
#include <smartcard++/PCSCManager.h>
#include <smartcard++/SCError.h>
#include <smartcard++/CardBase.h> //for exceptions

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
	SCError::check((*pSCardEstablishContext)(SCARD_SCOPE_USER,
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
#ifndef __APPLE__
	pSCardGetAttrib = (LONG(SCAPI *)(SCARDHANDLE,DWORD,LPBYTE,LPDWORD))
		mLibrary.getProc("SCardGetAttrib");
#endif
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
	pSCardControl=(LONG (SCAPI *)(SCARDHANDLE, DWORD, LPCVOID, DWORD, LPVOID, DWORD, LPDWORD))
		mLibrary.getProc("SCardControl");
	
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
	SCError::check((*pSCardListReaders)(mSCardContext,NULL,NULL,&ccReaders));
	if (ccReaders == 0) {
		mReaderStates.clear();
		return;
		}
	if (ccReaders != mReaders.size()) { //check whether we have listed already
		mReaderStates.clear();
		mReaders.resize(ccReaders);
		SCError::check((*pSCardListReaders)(mSCardContext,NULL,&mReaders[0],&ccReaders));
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
		SCError::check((*pSCardGetStatusChange)(mSCardContext, 0, &mReaderStates[i], 1));
	}
#else
	SCError::check((*pSCardGetStatusChange)(mSCardContext,0, &mReaderStates[0],DWORD(mReaderStates.size())));
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
	SCError::check((*pSCardReconnect)(pc->hScard, 
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
	
	SCError::check((*pSCardConnect)(mSCardContext, (CSTRTYPE) mReaderStates[idx].szReader,
		SCARD_SHARE_SHARED,
		(pc->mForceT0 ? 0 : SCARD_PROTOCOL_T1 ) | SCARD_PROTOCOL_T0
		, & pc->hScard,& pc->proto));
	// Is there a pinpad?
	pc->verify_ioctl = pc->verify_ioctl_start = pc->modify_ioctl = pc->modify_ioctl_start = 0;
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
}

void PCSCManager::deleteConnection(ConnectionBase *c)
{
	SCError::check((*pSCardDisconnect)((( PCSCConnection *)c)->hScard,SCARD_RESET_CARD));
}

void PCSCManager::beginTransaction(ConnectionBase *c)
{
	SCError::check((*pSCardBeginTransaction)( (( PCSCConnection *)c)->hScard));
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
			result = (*pSCardReconnect)((( PCSCConnection *)c)->hScard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, 
				SCARD_LEAVE_CARD,&active);
			(*pSCardStatus)((( PCSCConnection *)c)->hScard,reader,&rdrLen,&state,&proto,atr,&atrLen);
			}
		}
	/*SCError::check(*/
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
	SCError::check((*pSCardTransmit)(pc->hScard,
				pc->proto == SCARD_PROTOCOL_T0 ? &_MT0 : &_MT1 ,
				&cmd[0],(DWORD)cmd.size() ,
				NULL,
				&recv[0] , &ret));
	recvLen = (uint)(ret);
}

void PCSCManager::execPinCommand(ConnectionBase *c, DWORD ioctl, std::vector<byte> &cmd) {
	PCSCConnection *pc = (PCSCConnection *)c;
	int offset = 0, count = 0;
	BYTE sbuf[256], rbuf[256];
	DWORD ioctl2, slen, rlen=sizeof(rbuf);
	
	PIN_VERIFY_STRUCTURE *pin_verify  = (PIN_VERIFY_STRUCTURE *)sbuf;
	PIN_MODIFY_STRUCTURE *pin_modify  = (PIN_MODIFY_STRUCTURE *)sbuf;
	
	// build PC/SC block. FIXME: hardcoded for EstEID!!!
	if (ioctl == pc->verify_ioctl_start || ioctl == pc->verify_ioctl) {	
		/* PIN verification control message */
		pin_verify->bTimerOut = 30;
		pin_verify->bTimerOut2 = 30;
		pin_verify->bmFormatString |= 0x02;
		pin_verify->bmPINBlockString = 0x00;
		pin_verify->bmPINLengthFormat = 0x00;
		pin_verify->wPINMaxExtraDigit = (4 << 8 ) + 12; // little endian!
		pin_verify->bEntryValidationCondition = 0x02; /* Keypress only */
		pin_verify->bNumberMessage = pc->display ? 0xFF: 0x00; /* Default message */

		/* Ignore language and T=1 parameters. */
		pin_verify->wLangId = 0x0000;
		pin_verify->bMsgIndex = 0x00;
		pin_verify->bTeoPrologue[0] = 0x00;
		pin_verify->bTeoPrologue[1] = 0x00;
		pin_verify->bTeoPrologue[2] = 0x00;

		/* APDU itself */
		pin_verify->abData[offset++] = 0x00;
		pin_verify->abData[offset++] = 0x20;
		pin_verify->abData[offset++] = 0x00;
		pin_verify->abData[offset++] = 0x01;
		pin_verify->ulDataLength = 0x0004;
		count = sizeof(PIN_VERIFY_STRUCTURE) + offset -1;
	} else {
		pin_modify->bTimerOut = 30;
		pin_modify->bTimerOut2 = 30;
		pin_modify->bmFormatString |= 0x02;
		pin_modify->bmPINBlockString = 0x00;
		pin_modify->bmPINLengthFormat = 0x00;
		pin_modify->bInsertionOffsetOld = 0x00;
		pin_modify->bInsertionOffsetNew = 0x00;
		pin_modify->wPINMaxExtraDigit = (5 << 8 ) + 12;
		pin_modify->bConfirmPIN = 0x03;
		pin_modify->bEntryValidationCondition = 0x02;
		pin_modify->bNumberMessage = pc->display ? 0x03 : 0x00;
		pin_modify->wLangId =0x0000;
		pin_modify->bMsgIndex1 = 0x00;
		pin_modify->bMsgIndex2 = 0x01;
		pin_modify->bMsgIndex3 = 0x02;
		pin_modify->bTeoPrologue[0] = 0x00;
		pin_modify->bTeoPrologue[1] = 0x00;
		pin_modify->bTeoPrologue[2] = 0x00;

		/* APDU itself */
		pin_modify->abData[offset++] = 0x00;
		pin_modify->abData[offset++] = 0x24;
		pin_modify->abData[offset++] = 0x00;
		pin_modify->abData[offset++] = 0x02;
		pin_modify->ulDataLength = 0x0004; /* APDU size */
		count = sizeof(PIN_MODIFY_STRUCTURE) + offset -1;
	}
	SCError::check((*pSCardControl)(pc->hScard, ioctl, sbuf, sizeof(sbuf), rbuf, sizeof(rbuf), &rlen));

	// finish a two phase operation
	if ((ioctl == pc->verify_ioctl_start) || (ioctl == pc->modify_ioctl_start)) {
		rlen = sizeof(rbuf);
		ioctl2 = (ioctl == pc->verify_ioctl_start) ? pc->verify_ioctl_finish : pc->modify_ioctl_finish;
		SCError::check((*pSCardControl)(pc->hScard, ioctl2, NULL, 0, rbuf, sizeof(rbuf), &rlen));
	}
	byte SW1 = rbuf[rlen - 2];
	byte SW2 = rbuf[rlen - 1];
	if (SW1 != 0x90) {
		if (SW1==0x64 && ( SW2 == 0x00 || SW2 == 0x01 || SW2 == 0x02) ) {
			throw AuthError(SW1,SW2);
		}
		throw CardError(SW1, SW2);
	}
}

void PCSCManager::execPinEntryCommand(ConnectionBase *c,std::vector<byte> &cmd) {
	PCSCConnection *pc = (PCSCConnection *)c;
	execPinCommand(c, pc->verify_ioctl_start ? pc->verify_ioctl_start : pc->verify_ioctl, cmd);
}

void PCSCManager::execPinChangeCommand(ConnectionBase *c,std::vector<byte> &cmd, size_t oldPinLen, size_t newPinLen) {
	PCSCConnection *pc = (PCSCConnection *)c;
	execPinCommand(c, pc->modify_ioctl_start ? pc->modify_ioctl_start : pc->modify_ioctl, cmd);
}
         


bool PCSCManager::isT1Protocol(ConnectionBase *c) {
	PCSCConnection *pc = (PCSCConnection *)c;
	return pc->proto == SCARD_PROTOCOL_T1 && !pc->mForceT0;
	}
