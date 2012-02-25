/*
* SMARTCARDPP
* 
* This software is released under either the GNU Library General Public
* License (see LICENSE.LGPL) or the BSD License (see LICENSE.BSD).
* 
* Note that the only valid version of the LGPL license as far as this
* project is concerned is the original GNU Library General Public License
* Version 2.1, February 1999
*
*/

#include <cstdlib>
#include "common.h"
#include "CardBase.h"
#include <algorithm>
#include <time.h>
#include "helperMacro.h"
#ifdef WIN32
	#include <Windows.h>
	#include <WinNT.h>
	#include <Psapi.h>
#else
#include <unistd.h>
#endif

#define NULLWSTR(a) (a == NULL ? L"<NULL>" : a)

CardError::CardError(byte a,byte b):runtime_error("invalid condition on card")
	,SW1(a),SW2(b) {
	std::ostringstream buf;
	buf << "CardError:'" << runtime_error::what() << "'" << 
		" SW1:'0x" <<
		std::hex << std::setfill('0') <<
		std::setw(2) << ushort(a) << "'"
		" SW2:'0x" <<
		std::hex << std::setfill('0') <<
		std::setw(2) << ushort(b) << "'"
		;
	desc = buf.str();
}

CardBase::CardBase(ManagerInterface &ref) :
	mManager(ref),mConnection(NULL) {
		mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	}

CardBase::CardBase(ManagerInterface &ref,unsigned int idx) :
	mManager(ref)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	
	connect(idx);
}

CardBase::CardBase(ManagerInterface &ref,ConnectionBase *conn):
	mManager(ref),mConnection(conn)
	{
		mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
		//logFile = fopen("C:/Users/ITA/smartcardpp_custom.log", "a+");
	}

void CardBase::connect(unsigned int idx) {
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	mConnection = mManager.connect(idx);
	reIdentify();
}

CardBase::~CardBase(void)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	//fclose(logFile);
	if (mConnection) {
		delete mConnection;
		mConnection = NULL;
		}
}

ByteVec CardBase::getTag(int identTag,int len,ByteVec &arr)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	std::ostringstream oss;
	ByteVec::iterator iTag;
	iTag = find(arr.begin(),arr.end(),identTag);
	if (iTag == arr.end() )
	{
		oss <<  "fci tag not found, tag " << identTag;
		throw CardDataError( oss.str() );
		
	}
	if (len && *(iTag+1) != len)
	{
		oss <<  "fci tag " << identTag << " invalid length, expecting " <<
			len << " got " << int(*(iTag+1));
		throw CardDataError(oss.str());
	}

	return ByteVec(iTag + 2,iTag + 2 + *(iTag + 1));
}

CardBase::FCI CardBase::parseFCI(ByteVec fci)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	throw std::runtime_error("unimplemented");
}


CardBase::FCI CardBase::selectMF(bool ignoreFCI)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	byte cmdMF[]= {CLAByte(), 0xA4, 0x00, ignoreFCI ? 0x08 : 0x00, 0x00/*0x02,0x3F,0x00*/}; 
	ByteVec code;
	code = execute( MAKEVECTOR(cmdMF), ignoreFCI ? 2 : 4);
	if (ignoreFCI) return FCI();
	return parseFCI(code);;
}

int CardBase::selectDF(int fileID,bool ignoreFCI)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	byte cmdSelectDF[] = {CLAByte(),0xA4,0x01,ignoreFCI ? 0x08 : 0x04,0x02};
	ByteVec cmd(MAKEVECTOR(cmdSelectDF));
	cmd.push_back(HIBYTE(fileID));
	cmd.push_back(LOBYTE(fileID));
	ByteVec fcp =  execute(cmd, 4);
	if (ignoreFCI) return 0;
/*	FCI blah = */parseFCI(fcp);
	return 0;
}

CardBase::FCI CardBase::selectEF(int fileID,bool ignoreFCI)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	byte cmdSelectEF[] = {CLAByte(), 0xA4, 0x02, ignoreFCI ? 0x08 : 0x04, 0x02 };
	ByteVec cmd(MAKEVECTOR(cmdSelectEF));
	cmd.push_back(HIBYTE(fileID));
	cmd.push_back(LOBYTE(fileID));
	ByteVec fci = execute(cmd, 4);

	if (ignoreFCI)
		return FCI();
	return parseFCI(fci);
}

#define PACKETSIZE 254

ByteVec CardBase::readEF(unsigned int  fileLen) 
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	byte cmdReadEF[] = {CLAByte(),0xB0,0x00,0x00,0x00};
	ByteVec cmd(MAKEVECTOR(cmdReadEF));

	ByteVec read(0);
	dword i=0;
	
	do {
		byte bytes = LOBYTE( i + PACKETSIZE > fileLen ? fileLen - i : PACKETSIZE );
		
		cmd[2] = HIBYTE(i); //offsethi
		cmd[3] = LOBYTE(i); //offsetlo
		cmd[4] = bytes; //count

		ByteVec ret = execute(cmd);
		if ( bytes != ret.size() ) 
			throw CardDataError("less bytes read from binary file than specified");

		read.insert(read.end(), ret.begin(),ret.end());
		i += PACKETSIZE ;
	} while (i < (fileLen - 1));
	return read;
}

ByteVec CardBase::readRecord(int numrec) 
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	byte cmdReadREC[] = {CLAByte(),0xB2,0x00,0x04,0x00}; 
	cmdReadREC[2] = LOBYTE(numrec);
	return execute(MAKEVECTOR(cmdReadREC));
}

void CardBase::executePinEntry(ByteVec cmd)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	mManager.execPinEntryCommand(mConnection,cmd);
}

void CardBase::executePinChange(ByteVec cmd, size_t oldPinLen,size_t newPinLen)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	mManager.execPinChangeCommand(mConnection,cmd,oldPinLen,newPinLen);
}

void CardBase::setLogging(std::ostream *logStream)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	//mLogger = logStream;
}

ByteVec CardBase::execute(ByteVec cmd, int apducase)
{
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	ByteVec RecvBuffer(258); // 256 + SW
	byte SW1 = 0x00;
	byte SW2 = 0x00;
	std::string readerState = "";
	int retriesLeft = 10;
	time_t dbgTime;
	time(&dbgTime);
	struct tm *timeinfo = localtime(&dbgTime);
	uint realLen = (uint) RecvBuffer.size() ;
	std::stringstream APDU;

	// T=1 needs Le byte for case 4 APDU-s
	if (mManager.isT1Protocol(mConnection) && apducase == 4)
	{
		mManager.writeLog("[%s] [%d] Protocol is T1. Pushing back one byte", __FUNCTION__, __LINE__);
		cmd.push_back((byte) 0x00); // 0x00 == "All data available, up to 256 bytes". Sufficient for our uses.
	}

	mManager.writeLog("[%s] [%d] Executing APDU", __FUNCTION__, __LINE__);

	while((SW1 == 0x00 && SW2 == 0x00) && retriesLeft > 0)
	{
		APDU.str("");
		for (ByteVec::iterator it = cmd.begin(); it < cmd.end(); it++)
			APDU << std::hex << std::setfill('0') << std::setw(2) << (int) *it << " ";
		mManager.writeLog("[%s] [%d] -> (%s, %i) %s", __FUNCTION__, __LINE__, mManager.isT1Protocol(mConnection) ? "T1" : "T0", retriesLeft, APDU.str().c_str());

		if(retriesLeft < 10)
		{
			mManager.writeLog("[%s] [%d] Command send failed with status SW1 = 0x00 SW 2 == 0x00. Pausing and then will try again. Retry: %i ", __FUNCTION__, __LINE__, 10 - retriesLeft);

			#ifdef WIN32
				Sleep(3000);
			#else
				sleep(3);
			#endif

			mManager.writeLog("[%s] [%d] Reconnecting", __FUNCTION__, __LINE__);
			mManager.reconnect(mConnection);
		}
		mManager.writeLog("[%s] [%d] Executing", __FUNCTION__, __LINE__);
		mManager.execCommand(mConnection, cmd, RecvBuffer, realLen);
		mManager.writeLog("[%s] [%d] Recieved buffer size is: %i bytes", __FUNCTION__, __LINE__, realLen);
		if(realLen < 2)
		{
		  mManager.writeLog("[%s] [%d] Recieved buffer size is less then two bytes: %i", __FUNCTION__, __LINE__,  realLen);
		  SW1 = 0x00;
		  SW2 = 0x00;
		  throw CardDataError("Zero-length input from cardmanager");
		}

		APDU.str("");
		for (ByteVec::iterator it = RecvBuffer.begin(); it < RecvBuffer.end(); it++)
			APDU << std::hex << std::setfill('0') << std::setw(2) << (int) *it << " ";
		mManager.writeLog("[%s] [%d] <- (%s, %i) %s", __FUNCTION__, __LINE__, mManager.isT1Protocol(mConnection) ? "T1" : "T0", retriesLeft, APDU.str().c_str());

		mManager.writeLog("[%s] [%d] Executed", __FUNCTION__, __LINE__);
		SW1 = RecvBuffer[realLen - 2];
		SW2 = RecvBuffer[realLen - 1];
		mManager.writeLog("[%s] [%d] SW1: %02x SW2: %02x", __FUNCTION__, __LINE__, SW1, SW2);

		// chop off SW
		if(realLen > 2)
		  RecvBuffer.resize(realLen - 2);
		mManager.writeLog("[%s] [%d] Buffer resized", __FUNCTION__, __LINE__);

		//if (realLen < 2)
		//	throw std::runtime_error("zero-length input from cardmanager");

		int retriesCount = 0;
		while(SW1 == 0x61)
		{
			mManager.writeLog("[%s] [%d] Executing retry command: %i", __FUNCTION__, __LINE__, retriesCount);
			RecvBuffer.resize(258);
	
			realLen = (uint) RecvBuffer.size();
			byte cmdRead[]= {CLAByte(),0xC0,0x00,0x00,0x00};
			cmdRead[4] = SW2;
			ByteVec cmdReadVec = MAKEVECTOR(cmdRead);

			APDU.str("");
			for (ByteVec::iterator it = cmdReadVec.begin(); it < cmdReadVec.end(); it++)
				APDU << std::hex << std::setfill('0') << std::setw(2) << (int) *it << " ";
			mManager.writeLog("[%s] [%d] -> (%s, %i) %s", __FUNCTION__, __LINE__, mManager.isT1Protocol(mConnection) ? "T1" : "T0", retriesLeft, APDU.str().c_str());

			mManager.writeLog("[%s] [%d] Executing...", __FUNCTION__, __LINE__);
			mManager.execCommand(mConnection, cmdReadVec, RecvBuffer, realLen);
			mManager.writeLog("[%s] [%d] Recieved buffer size is: %i bytes", __FUNCTION__, __LINE__, realLen);
			if(realLen < 2)
			{
			  mManager.writeLog("[%s] [%d] Recieved buffer size is less then two bytes: %i", __FUNCTION__, __LINE__,  realLen);
			  SW1 = 0x00;
			  SW2 = 0x00;
			  throw CardDataError("Zero-length input from cardmanager");
			}

			APDU.str("");
			for (ByteVec::iterator it = RecvBuffer.begin(); it < RecvBuffer.end(); it++)
				APDU << std::hex << std::setfill('0') << std::setw(2) << (int) *it << " ";
			mManager.writeLog("[%s] [%d] <- (%s, %i) %s", __FUNCTION__, __LINE__, mManager.isT1Protocol(mConnection) ? "T1" : "T0", retriesLeft, APDU.str().c_str());
			mManager.writeLog("[%s] [%d] Executed...", __FUNCTION__, __LINE__);

			SW1 = RecvBuffer[realLen - 2];
			SW2 = RecvBuffer[realLen - 1];
			mManager.writeLog("[%s] [%d] SW1: %02x SW2: %02x", __FUNCTION__, __LINE__, SW1, SW2);
			if(realLen > 2)
			  RecvBuffer.resize(realLen - 2);
			mManager.writeLog("[%s] [%d] Retry buffer resized", __FUNCTION__, __LINE__);

		//	if (realLen < 2)
		//		throw std::runtime_error("zero-length input from cardmanager");

			retriesCount++;
		}
		mManager.writeLog("[%s] [%d] Going to next iteraction", __FUNCTION__, __LINE__);
		retriesLeft--;
	}
	if(retriesLeft <= 0)
	{
		mManager.writeLog("[%s] [%d] Card was reset more than 10 times. Command execution failed.", __FUNCTION__, __LINE__);
		throw CardDataError("Card is in use by some other process");
	}

	if (!(SW1 == 0x90 && SW2 == 0x00))
	{
		mManager.writeLog("[%s] [%d] Error: SW1 %02x SW2 %02x", __FUNCTION__, __LINE__, SW1, SW2);
		throw CardError(SW1,SW2);
	}
	mManager.writeLog("[%s] [%d] Rerturning OK", __FUNCTION__, __LINE__);
	
	return RecvBuffer;
}


void CardBase::endTransaction() {
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	mManager.endTransaction(mConnection,true);
	}

bool CardBase::hasSecurePinEntry() {
	mManager.writeLog("[%s] [%d]", __FUNCTION__, __LINE__);
	return mConnection->isSecure();
	}
