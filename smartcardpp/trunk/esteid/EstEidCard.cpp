/*!
	\file		EstEidCard.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2010-02-04 08:10:53 +0200 (N, 04 veebr 2010) $
*/
// Revision $Revision: 512 $
#include "EstEidCard.h"
#include "SCError.h"
#include "helperMacro.h"
#include "common.h"
#include <algorithm>
#include <functional>
#include <cctype>
#include <iostream>
#include <string>

#include <string.h>

#define tagFCP 0x62 //file control parameters
#define tagFMD 0x64 //file management data
#define tagFCI 0x6F //file control information

#define SC_MAX_ATR_SIZE 33

using std::string;

namespace
{

EstEidCard::CardVersion atr_to_version(string &ATR)
{
	std::transform(ATR.begin(), ATR.end(), ATR.begin(), ::toupper);

	// ID-kaart 1.0
	if (ATR == "3BFE9400FF80B1FA451F034573744549442076657220312E3043" ||
		ATR == "3B6E00FF4573744549442076657220312E30")
		return EstEidCard::VER_1_0;

	// ID-kaart 1.0 2007
	if (ATR == "3BDE18FFC080B1FE451F034573744549442076657220312E302B" ||
		ATR == "3B5E11FF4573744549442076657220312E30")
		return EstEidCard::VER_1_0_2007;

	// Digi-ID 1.1 MULTOS
	if (ATR == "3B6E00004573744549442076657220312E30")
		return EstEidCard::VER_1_1;

	// ID-kaart 3.0 2011 2k JavaCard dev1
	if (ATR == "3BFE1800008031FE454573744549442076657220312E30A8" ||
		ATR == "3BFE1800008031FE45803180664090A4561B168301900086")
		return EstEidCard::VER_3_0;

	// ID-kaart 3.0 2011 2k JavaCard dev2 warm
	if (ATR == "3BFE1800008031FE45803180664090A4162A0083019000E1")
		return EstEidCard::VER_3_0;

	// ID-kaart 3.0 2011 2k JavaCard production warm (18.01.2011+)
	if (ATR == "3BFE1800008031FE45803180664090A4162A00830F9000EF")
		return EstEidCard::VER_3_0;
	
	return EstEidCard::VER_INVALID;
}

}

bool EstEidCard::isInReader(unsigned int idx)
{
	string ATR = mManager.getATRHex(idx);
	_card_version = atr_to_version(ATR);

	return _card_version != VER_INVALID;
}

void EstEidCard::setVersion()
{
	byte atr[SC_MAX_ATR_SIZE];
	dword atr_size = sizeof(atr);
	memset(atr, 0, atr_size);

	mManager.getATRHex(mConnection, atr, &atr_size);

	// FIXME: reinterpret_cast from unsigned char to char may mess up bytes
	// hex bytes should be safe though
	std::string atr_str(reinterpret_cast<const char *>(atr), atr_size);
	_card_version = atr_to_version(atr_str);

	if (_card_version == VER_INVALID)
		// FIXME: refactor exceptions
		throw std::runtime_error("Unknown ATR: " + atr_str);
}

unsigned int EstEidCard::getKeySize()
{
	// The card is constructed with invalid version,
	// lazy load the real version, but only once.
	if (_card_version == VER_INVALID)
		setVersion();

	if (_card_version == VER_3_0)
		return 2048;

	return 1024;
}

CardBase::FCI EstEidCard::parseFCI(ByteVec fci)
{
	ByteVec tag;

	FCI tmp;
	tmp.fileID = 0;
	if (fci.size() < 0x0B ||
		(fci[0] != tagFCP && fci[0] != tagFCI)	||
		fci.size()-2 != fci[1] )
		throw CardDataError("invalid fci record");

	if (getCardVersion() == VER_3_0)
		fci = ByteVec(fci.begin()+ 4, fci.end());
	else
		fci = ByteVec(fci.begin()+ 2, fci.end());

	tag = getTag(0x83, 2, fci);
	if (tag.size() != 2)
		throw CardDataError("file name record invalid length, not two bytes");

	tmp.fileID = MAKEWORD(tag[1],tag[0]);

	tag = getTag(0x82, 0, fci);
	switch (tag[0] & 0x3F) {
		case 0x38: //DF
		case 0x01: //binary
			if (tag.size() != 1)
				throw CardDataError("linear variable file descriptor not 1 bytes long");
			tag = getTag(0x85,2,fci);
			tmp.fileLength = MAKEWORD(tag[1],tag[0]);
			break;
		case 0x02:
		//linear variable
		case 0x04:
			if (tag.size() != 5)
				throw CardDataError("linear variable file descriptor not 5 bytes long");
			tmp.recMaxLen	= MAKEWORD( tag[0x03], tag[0x02] );
			tmp.recCount	= tag[0x04];
			tmp.fileLength	= 0;
			break;

		default:
			throw CardDataError("invalid filelen record, unrecognized tag");
		}
	return tmp;
}

bool EstEidCard::isSecureConnection() {
	return mConnection && mConnection->isSecure();
}

void EstEidCard::enterPin(PinType pinType, PinString pin, bool forceUnsecure) {
	byte cmdEntPin[] = {0x00,0x20,0x00}; // VERIFY
	ByteVec cmd(MAKEVECTOR(cmdEntPin));
	cmd.push_back((byte)pinType);
	if (pin.length() < 4) {
		if (pin.length()!= 0 ||!mConnection->isSecure() ) {
			AuthError err(0,0);
			err.m_badinput = true;
			err.desc = "bad pin length";
			throw err;
		}
	} else {
		cmd.push_back(LOBYTE(pin.length()));
		for (unsigned i = 0; i < pin.length(); i++)
			cmd.push_back(pin[i]);
		forceUnsecure = true; // Use normal command for sending PIN
	}
	try {
		if (forceUnsecure || !mConnection->isSecure())
			execute(cmd,true); //exec as a normal command
		else
			executePinEntry(cmd);
	} catch(AuthError &ae) {
		throw AuthError(ae);
	} catch(CardError &e) {
		if (e.SW1 == 0x63) throw AuthError(e);
		if (e.SW1 == 0x69 && e.SW2 == 0x83) throw AuthError(e.SW1,e.SW2,true);
		throw e;
	}
}

void EstEidCard::setSecEnv(byte env)
{
	byte cmdSecEnv1[] = {0x00, 0x22, 0xF3};
	ByteVec cmd(MAKEVECTOR(cmdSecEnv1));
	cmd.push_back(env);
	execute(cmd, true);
}

void EstEidCard::prepareSign_internal(KeyType keyId, PinString pin)
{
	selectMF(true);
	selectDF(FILEID_APP,true);

	if (keyId == 0)
		enterPin(PIN_AUTH,pin);
	else
		enterPin(PIN_SIGN,pin);
}

ByteVec EstEidCard::calcSign_internal(AlgType type,KeyType keyId,ByteVec hash,bool withOID)
{
	byte signCmdSig[] = {0x00,0x2A,0x9E,0x9A};
	byte signCmdAut[] = {0x00,0x88,0x00,0x00};
	byte hashHdMD5[] = {0x30,0x20,0x30,0x0C,0x06,0x08,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x02,0x05,0x05,0x00,0x04,0x10};
	byte hashHdSHA1[] = {0x30,0x21,0x30,0x09,0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x05,0x00,0x04,0x14};

	ByteVec cmd,header;
	if (keyId == 0 )
		cmd = MAKEVECTOR(signCmdAut);
	else
		cmd = MAKEVECTOR(signCmdSig);

	if (withOID) {
		switch(type) {
			case MD5:
				header = MAKEVECTOR(hashHdMD5);
				break;
			case SHA1:
				header = MAKEVECTOR(hashHdSHA1);
				break;
			default:
				throw std::runtime_error("cannot calculate SSL signature with OID");
			}
		cmd.push_back((byte) (header.size() + hash.size()));
		cmd.insert(cmd.end(), header.begin(), header.end());
	}
	else
		cmd.push_back((byte)hash.size());

	setSecEnv(1);

	cmd.insert(cmd.end(), hash.begin(), hash.end());
	ByteVec result;
	try {
		result = execute(cmd);
	} catch(CardError e) {
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 ))
			throw AuthError(e);
		throw e;
		}
	return result;
}

ByteVec EstEidCard::RSADecrypt_internal(ByteVec cipher)
{
	byte modEnv1[] = {0x00,0x22,0x41,0xA4,0x02,0x83,0x00};
	byte modEnv2[] = {0x00,0x22,0x41,0xB6,0x02,0x83,0x00};
	byte modEnv0[] = {0x00,0x22,0x41,0xB8,0x05,0x83,0x03,0x80};

	selectMF(true);
	selectDF(FILEID_APP,true);
	selectEF(FILEID_KEYPOINTER,true);
	ByteVec keyRec = readRecord(1);
	if (keyRec.size() != 0x15)
		throw CardDataError("key ptr len is not 0x15");
	ByteVec cmd(MAKEVECTOR(modEnv0));
	cmd.insert(cmd.end(),&keyRec[0x9],&keyRec[0xB]);

	setSecEnv(6);
	execute(MAKEVECTOR(modEnv1),true);
	execute(MAKEVECTOR(modEnv2),true);
	execute(cmd,true);

	byte decrpt[] = {0x00,0x2A,0x80,0x86,0x81,0x00};
	ByteVec decCmd(MAKEVECTOR(decrpt));
	decCmd.insert(decCmd.end(),cipher.begin(),cipher.end());

	ByteVec result;
	try {
		result = execute(decCmd,false);
	} catch(CardError e) {
/*		if (e.SW1 == 0x69 && e.SW2 == 0x88 ) {
			reverse(decCmd.begin() + 6, decCmd.end());
			result = execute(decCmd,false);
			return result;
			}*/
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x88  || e.SW2 == 0x85
			/*|| e.SW1 == 0x64 */|| e.SW1 == 0x6B ))
			throw AuthError(e);
		throw e;
		}
	return result;
}

void EstEidCard::readPersonalData_internal(vector<string>& data,int recStart,int recEnd)
{
	selectMF(true);
	selectDF(FILEID_APP,true);
	FCI fileInfo = selectEF(0x5044);

	if (fileInfo.recCount != 0x10 && fileInfo.recCount != 0x0A && fileInfo.recCount != 0x14)
		throw CardDataError("personal data file does not contain 16 or 10 records");

	data.resize(std::min(recEnd,0x10) + 1 );
	for (int i=recStart; i <= recEnd; i++) {
		ByteVec record = readRecord(i);
		string& ret = data[i];
		for (ByteVec::iterator c = record.begin();c!=record.end();c++)
			ret+= *c ;
		/* EstEID 3.0 has fixed length, space padded data file records. Trim from the end */
		if (getCardVersion() == VER_3_0)
			ret.erase(std::find_if(ret.rbegin(), ret.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), ret.end());
	}
}

bool EstEidCard::validatePin_internal(PinType pinType,PinString pin, byte &retriesLeft,bool forceUnsecure) {
	UNUSED_ARG(forceUnsecure);
	checkProtocol();
	selectMF(true);
	if (retriesLeft != 0xFA ) { //sorry, thats a bad hack around sloppy interface definition
		retriesLeft = 0;
		selectEF(FILEID_RETRYCT,true);
		ByteVec data = readRecord(pinType == PUK ? 3 : pinType );
		ByteVec tag = getTag(0x90,1,data);
		retriesLeft = tag[0];
		}
	selectDF(FILEID_APP,true);
	enterPin(pinType,pin);
	return true;
	}

bool EstEidCard::changePin_internal(
	PinType pinType,PinString newPin,PinString oldPin,bool useUnblockCmd ) {
	byte cmdChangeCmd[] = {0x00,0x24,0x00};
	bool doSecure = false;

	if (useUnblockCmd) cmdChangeCmd[1]= 0x2C;
	ByteVec cmd(MAKEVECTOR(cmdChangeCmd));
	cmd.push_back((byte)pinType);

	size_t oldPinLen,newPinLen;
	if (newPin.length() < 4 || oldPin.length() < 4 ) {
		if (!mConnection->isSecure() )
			throw std::runtime_error("bad pin length");

		// FIXME: This is probably broken on PC/SC readers and
		//	  it is not known to work on CT-API either
		oldPinLen = (oldPin[0] - '0') * 10 + oldPin[1] - '0';
		newPinLen = (newPin[0] - '0') * 10 + newPin[1]- '0';
		oldPin = PinString(oldPinLen,'0');
		newPin = PinString(newPinLen,'0');
		doSecure = true;
	} else {
		oldPinLen = oldPin.length();
		newPinLen = newPin.length();
	}

	ByteVec catPins;
	catPins.resize(oldPinLen + newPinLen);
	copy(oldPin.begin(), oldPin.end(), catPins.begin());
	copy(newPin.begin(), newPin.end(), catPins.begin() + oldPin.length());
	cmd.push_back(LOBYTE(catPins.size()));
	cmd.insert(cmd.end(),catPins.begin(),catPins.end());

	try {
		if (doSecure)
			executePinChange(cmd,oldPinLen,newPinLen);
		else
			execute(cmd,true);
	} catch(AuthError &ae) {
		throw AuthError(ae);
	} catch(CardError &e) {
		if (e.SW1 == 0x63)
			throw AuthError(e);
		else if (useUnblockCmd && e.SW1==0x6a && e.SW2 == 0x80 ) //unblocking, repeating old pin
			throw AuthError(e.SW1,e.SW2,true);
		else
			throw e;
		}

	return true;
	}

void EstEidCard::reconnectWithT0() {
	if (mConnection->mOwnConnection) {
		uint prev = mConnection->mIndex;
		delete mConnection;
		connect(prev);
	} else {
		mConnection = mManager.reconnect(mConnection);
	}
}

void EstEidCard::checkProtocol()
{
	try {
		selectMF(true);
	} catch(CardError &ce) {
		if (ce.SW1 != 0x6A || ce.SW2 != 0x87 ) throw;
		reconnectWithT0();
	} catch(SCError &sce) {
		if (sce.error != 0x1      //incorrect function, some drivers do that
			&& sce.error != 0x6f7 //microsoft generic PCSC with OK3621
			) throw;
		reconnectWithT0();
	}
}

//transacted, public methods
string EstEidCard::readCardID()
{
	vector<string> temp;
	Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(temp,ID,ID);
	string ret = temp[ID];
	return ret;
}

string EstEidCard::readDocumentID()
{
	vector<string> temp;
	Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(temp,DOCUMENTID,DOCUMENTID);

	string ret = temp[DOCUMENTID];
	return ret;
}	

string EstEidCard::readCardName(bool firstNameFirst)
{
	// FIXME: fetch something from certificate?
	if (getCardVersion() == VER_1_1)
		return "Digi ID";

	vector<string> temp;
	Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(temp,SURNAME,FIRSTNAME);

	string ret;
	if (firstNameFirst)
		ret = temp[FIRSTNAME] + " " + temp[SURNAME];
	else
		ret = temp[SURNAME] + " " + temp[FIRSTNAME];
	return ret;
}

void EstEidCard::reIdentify()
{
	byte cmdGetVersion[] = {0x00, 0xCA, 0x01, 0x00, 0x02};
	ByteVec cmd(MAKEVECTOR(cmdGetVersion));

	ByteVec result;
	try {
		result = execute(cmd);
	} catch(...) { return; }
	
	if (result[0] == 0x01 && result[1] == 0x01)
		_card_version = VER_1_1;
	else if (result[0] == 0x03 && result[1] == 0x00)
		_card_version = VER_3_0;
}

bool EstEidCard::readPersonalData(vector<string>& data, int firstRecord,int lastRecord)
{
	Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(data,firstRecord,lastRecord);
	return true;
}

bool EstEidCard::getKeyUsageCounters(dword &authKey,dword &signKey)
{
	Transaction _m(mManager,mConnection);
	selectMF(true);
	selectDF(FILEID_APP,true);

	selectEF(FILEID_KEYPOINTER,true);
	ByteVec keyRec = readRecord(1);
	if (keyRec.size() != 0x15)
			throw CardDataError("key ptr len is not 0x15");
	byte UsedKey = 0;
	if (keyRec[0x9]== 0x12 && keyRec[0xA] == 0x00 ) UsedKey++;

	FCI fileInfo = selectEF(0x0013);
	if (fileInfo.recCount < 4)
			throw CardDataError("key info file 0x13 should have 4 records");

	ByteVec authData = readRecord(UsedKey + 3);
	ByteVec signData = readRecord(UsedKey + 1);
	ByteVec aTag = getTag(0x91,3,authData);
	ByteVec sTag = getTag(0x91,3,signData);

	authKey = 0xFFFFFF - ((aTag[0] << 16) + (aTag[1] << 8) + aTag[2]);
	signKey = 0xFFFFFF - ((sTag[0] << 16) + (sTag[1] << 8) + sTag[2]);

	return true;
}

ByteVec EstEidCard::readEFAndTruncate(unsigned int fileLen)
{
	ByteVec ret = readEF(fileLen);
	if (ret.size() > 128) { //assume ASN sequence encoding with 2-byte length
		size_t realSize = ret[2] * 256 + ret[3] + 4;
		ret.resize(realSize);
	}
	return ret;
}

ByteVec EstEidCard::getAuthCert()
{
	Transaction _m(mManager,mConnection);

	selectMF(true);
	selectDF(FILEID_APP,true);
	FCI fileInfo = selectEF(0xAACE);

	return readEFAndTruncate(fileInfo.fileLength );
}

ByteVec EstEidCard::getSignCert()
{
	Transaction _m(mManager,mConnection);

	selectMF(true);
	selectDF(FILEID_APP,true);
	FCI fileInfo = selectEF(0xDDCE);
	return readEFAndTruncate(fileInfo.fileLength );
}

ByteVec EstEidCard::calcSSL(ByteVec hash) {
	Transaction _m(mManager,mConnection);

	return calcSign_internal(SSL,AUTH,hash,false);
}

ByteVec EstEidCard::calcSSL(ByteVec hash,PinString pin) {
	Transaction _m(mManager,mConnection);

	prepareSign_internal(AUTH,pin);
	return calcSign_internal(SSL,AUTH,hash,false);
	}

ByteVec EstEidCard::calcSignSHA1(ByteVec hash,KeyType keyId,bool withOID) {
	Transaction _m(mManager,mConnection);

	return calcSign_internal(SHA1,keyId,hash,withOID);
	}

ByteVec EstEidCard::calcSignSHA1(ByteVec hash,KeyType keyId,PinString pin,bool withOID) {
	Transaction _m(mManager,mConnection);

	prepareSign_internal(keyId,pin);
	return calcSign_internal(SHA1,keyId,hash,withOID);
	}

ByteVec EstEidCard::calcSignMD5(ByteVec hash,KeyType keyId,bool withOID) {
	Transaction _m(mManager,mConnection);

	return calcSign_internal(MD5,keyId,hash,withOID);
	}

ByteVec EstEidCard::calcSignMD5(ByteVec hash,KeyType keyId,PinString pin,bool withOID) {
	Transaction _m(mManager,mConnection);

	prepareSign_internal(keyId,pin);
	return calcSign_internal(MD5,keyId,hash,withOID);
	}

ByteVec EstEidCard::RSADecrypt(ByteVec cipher) {
	Transaction _m(mManager,mConnection);

	checkProtocol();
	return RSADecrypt_internal(cipher);
	}

ByteVec EstEidCard::RSADecrypt(ByteVec cipher,PinString pin) {
	Transaction _m(mManager,mConnection);

	selectMF(true);
	selectDF(FILEID_APP,true);
	enterPin(PIN_AUTH,pin);
	return RSADecrypt_internal(cipher);
	}

bool EstEidCard::validateAuthPin(PinString pin, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	return validatePin_internal(PIN_AUTH,pin,retriesLeft);
	}

bool EstEidCard::validateSignPin(PinString pin, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	return validatePin_internal(PIN_SIGN,pin,retriesLeft);
}

bool EstEidCard::validatePuk(PinString pin, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	return validatePin_internal(PUK,pin,retriesLeft);
}

bool EstEidCard::changeAuthPin(PinString newPin,PinString oldPin, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PIN_AUTH,oldPin,retriesLeft);
	else {
		byte dummy;
		getRetryCounts_internal(dummy,retriesLeft,dummy);
		}
	return changePin_internal(PIN_AUTH,newPin,oldPin);
	}

bool EstEidCard::changeSignPin(PinString newPin,PinString oldPin, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PIN_SIGN,oldPin,retriesLeft);
	else {
		byte dummy;
		getRetryCounts_internal(dummy,dummy,retriesLeft);
		}
	return changePin_internal(PIN_SIGN,newPin,oldPin);
	}

bool EstEidCard::changePUK(PinString newPUK,PinString oldPUK, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PUK,oldPUK,retriesLeft);
	else {
		byte dummy;
		getRetryCounts_internal(retriesLeft,dummy,dummy);
		}
	return changePin_internal(PUK,newPUK,oldPUK);
	}

bool EstEidCard::unblockAuthPin(PinString newPin,PinString mPUK, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PUK,mPUK,retriesLeft);
	if (newPin.length() < 4)
		throw std::runtime_error("secure unblock not implemented yet");
	for(char i='0'; i < '6'; i++) try { //ENSURE its blocked
			validatePin_internal(PIN_AUTH,PinString("0000") + i ,retriesLeft,true);
		} catch (...) {}
	return changePin_internal(PIN_AUTH,newPin,mPUK,true);
	}

bool EstEidCard::unblockSignPin(PinString newPin,PinString mPUK, byte &retriesLeft ) {
	Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PUK,mPUK,retriesLeft);
	if (newPin.length() < 4)
		throw std::runtime_error("secure unblock not implemented yet");
	for(char i='0'; i < '6'; i++) try { //ENSURE its blocked
			validatePin_internal(PIN_SIGN,PinString("0000") + i ,retriesLeft,true);
		} catch (...) {}
	return changePin_internal(PIN_SIGN,newPin,mPUK,true);
	}

bool EstEidCard::getRetryCounts(byte &puk,byte &pinAuth,byte &pinSign) {
	Transaction _m(mManager,mConnection);
	return getRetryCounts_internal(puk,pinAuth,pinSign);
	}

bool EstEidCard::getRetryCounts_internal(byte &puk,byte &pinAuth,byte &pinSign) {
	ByteVec data,tag;
	selectMF(true);
	selectEF(FILEID_RETRYCT,true);
	data = readRecord(3);
	tag = getTag(0x90,1,data);
	puk = tag[0];
	data = readRecord(PIN_AUTH);
	tag = getTag(0x90,1,data);
	pinAuth = tag[0];
	data = readRecord(PIN_SIGN);
	tag = getTag(0x90,1,data);
	pinSign = tag[0];
	return true;
	}

void EstEidCard::resetAuth() {
	try {
		Transaction _m(mManager,mConnection);
		selectMF(true);
		setSecEnv(0);
	} catch (...) {}
}

ByteVec EstEidCard::cardChallenge() {
	byte cmdEntPin[] = {0x00,0x84,0x00,0x00,0x08};
	ByteVec cmd(MAKEVECTOR(cmdEntPin));

	ByteVec result;
	try {
		result = execute(cmd);
	} catch(CardError e) {
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 )) 
			throw AuthError(e);
		throw e;
		}
	return result;
}

ByteVec EstEidCard::runCommand( ByteVec vec ) {
	ByteVec result;

	int retry = 3;
	while( retry )
	{
		try {
			result = execute(vec);
			break;
		} catch(CardError e) {
			if ( retry == 1 )
			{
				if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 )) 
					throw AuthError(e);
				throw e;
			} else
				reconnectWithT0();
		}
		retry--;
	}

	return result;
}
