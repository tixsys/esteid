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



#include "common.h"
#include "EstEidCard.h"
#include "helperMacro.h"
#include "SCError.h"
#include <algorithm>
#include <functional>
#include <cctype>
#include <iostream>
#include <string>
#include <string.h>
#ifdef WIN32
	#include "md5.h"
#endif
#define tagFCP 0x62 //file control parameters
#define tagFMD 0x64 //file management data
#define tagFCI 0x6F //file control information

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
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	string ATR = mManager.getATRHex(idx);
	_card_version = atr_to_version(ATR);

	return _card_version != VER_INVALID;
}

void EstEidCard::setVersion()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	string ATR = mManager.getATRHex(mConnection);
	_card_version = atr_to_version(ATR);

	if (_card_version == VER_INVALID)
		// FIXME: refactor exceptions
		throw std::runtime_error("Unknown ATR: " + ATR);
}

unsigned int EstEidCard::getKeySize()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
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
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	ByteVec tag;

	FCI tmp;
	tmp.fileID = 0;
	if (fci.size() < 0x0B ||
		(fci[0] != tagFCP && fci[0] != tagFCI)	||
		fci.size()-2 != fci[1] )
		throw CardDataError("invalid fci record");

/*	if (getCardVersion() == VER_3_0)
		fci = ByteVec(fci.begin()+ 4, fci.end());
	else
	*/
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
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	return mConnection && mConnection->isSecure();
}

void EstEidCard::enterPin(PinType pinType, PinString pin, bool forceUnsecure)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	byte cmdEntPin[] = {0x00,0x20,0x00}; // VERIFY
	ByteVec cmd(MAKEVECTOR(cmdEntPin));
	cmd.push_back((byte)pinType);
	if (pin.length() < 4) {
		if (pin.length()!= 0 ||!mConnection->isSecure() ) {
			AuthError err(0,0);
			err.m_badinput = true;
			err.desc = "bad pin length";
			mManager.endTransaction(mConnection);
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
			execute(cmd); //exec as a normal command
		else
			executePinEntry(cmd);
	} catch(AuthError &ae) {
		mManager.endTransaction(mConnection);
		throw AuthError(ae);
	} catch(CardError &e) {
		if (e.SW1 == 0x63) {mManager.endTransaction(mConnection); throw AuthError(e);}
		if (e.SW1 == 0x69 && e.SW2 == 0x83){ mManager.endTransaction(mConnection); throw AuthError(e.SW1,e.SW2,true);}
		if (e.SW1 == 0x69 && e.SW2 == 0x84)
		{
			AuthError err(0,0);
			err.m_badinput = true;
			err.desc = "Referenced data invalidated";
			mManager.endTransaction(mConnection);
			throw err;
		}
		mManager.endTransaction(mConnection);
		throw e;
	}
	mManager.endTransaction(mConnection);
}

void EstEidCard::setSecEnv(byte env)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	byte cmdSecEnv1[] = {0x00, 0x22, 0xF3};
	ByteVec cmd(MAKEVECTOR(cmdSecEnv1));
	cmd.push_back(env);
	execute(cmd);
	mManager.endTransaction(mConnection);
}

void EstEidCard::prepareSign_internal(KeyType keyId, PinString pin)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	selectMF(true);
	selectDF(FILEID_APP,true);

	if (keyId == 0)
		enterPin(PIN_AUTH,pin);
	else
		enterPin(PIN_SIGN,pin);
	mManager.endTransaction(mConnection);
}

ByteVec EstEidCard::calcSign_internal(AlgType type,KeyType keyId,ByteVec hash,bool withOID)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	//mManager.beginTransaction(mConnection);
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
				{
					throw std::runtime_error("cannot calculate SSL signature with OID");
					//mManager.endTransaction(mConnection);
				}
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
//#ifdef _WIN32
		//setVersion();//SIGSEGs Under Linux. Needed only for Windows MiniDriver
//#endif
		if(_card_version != VER_3_0)
		{
			mManager.writeLog("[%s:%d] Sending APDU case 4 (Card version %i)", __FUNCTION__, __LINE__, getCardVersion());
			result = execute(cmd, 4);
		}
		else
		{
			mManager.writeLog("[%s:%d] Sending default APDU case (Card version %i)", __FUNCTION__, __LINE__, getCardVersion());
			result = execute(cmd);
		}
	}
	catch(CardError e)
	{
		//mManager.endTransaction(mConnection);
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 ))
			throw AuthError(e);
		throw e;
	}
	//mManager.endTransaction(mConnection);
	return result;
}

// Called within a transaction
ByteVec EstEidCard::RSADecrypt_internal(ByteVec cryptogram)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	if (_card_version == VER_3_0 && cryptogram.size() != 256)
		throw std::runtime_error("2048 bit keys but cryptogram is not 256 bytes!");
	if (_card_version != VER_3_0 && cryptogram.size() != 128)
		throw std::runtime_error("cryptogram is not 128 bytes!");

	byte modEnv1[] = {0x00,0x22,0x41,0xA4,0x02,0x83,0x00};
	byte modEnv2[] = {0x00,0x22,0x41,0xB6,0x02,0x83,0x00};
	byte modEnv0[] = {0x00,0x22,0x41,0xB8,0x05,0x83,0x03,0x80};

	mManager.beginTransaction(mConnection);
	selectMF(true);
	selectDF(FILEID_APP,true);
	selectEF(FILEID_KEYPOINTER,true);
	ByteVec keyRec = readRecord(1);
	if (keyRec.size() != 0x15)
	{
		mManager.endTransaction(mConnection);
		throw CardDataError("key ptr len is not 0x15");
	}
	ByteVec cmd(MAKEVECTOR(modEnv0));
	cmd.insert(cmd.end(),&keyRec[0x9],&keyRec[0xB]);

	setSecEnv(6);
	execute(MAKEVECTOR(modEnv1));
	execute(MAKEVECTOR(modEnv2));
	execute(cmd);

	ByteVec result;

	try
	{
		if (_card_version == VER_3_0) {
			// Use chaining for decryption
			byte decrypt_chain1[] = {0x10, 0x2A, 0x80, 0x86, 0xFF, 0x00};
			byte decrypt_chain2[] = {0x00, 0x2A, 0x80, 0x86, 0x02};
			ByteVec first_command(MAKEVECTOR(decrypt_chain1));
			ByteVec second_command(MAKEVECTOR(decrypt_chain2));
			first_command.insert(first_command.end(), cryptogram.begin(), cryptogram.end()-2);
			second_command.insert(second_command.end(), cryptogram.end()-2, cryptogram.end());
			execute(first_command, 2);
			result = execute(second_command, 4);
		} else {
			byte decrypt[] = {0x00, 0x2A, 0x80, 0x86, 0x81, 0x00};
			ByteVec decrypt_command(MAKEVECTOR(decrypt));
			decrypt_command.insert(decrypt_command.end(), cryptogram.begin(), cryptogram.end());
			result = execute(decrypt_command, 4);
		}
	}
	catch(CardError e)
	{
		mManager.endTransaction(mConnection);
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x88  || e.SW2 == 0x85
			/*|| e.SW1 == 0x64 */|| e.SW1 == 0x6B ))
			throw AuthError(e);
		throw e;
	}
	mManager.endTransaction(mConnection);
	return result;
}


void EstEidCard::readPersonalData_internal(vector<string>& data,int recStart,int recEnd)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	//mManager.beginTransaction(mConnection);
	selectMF(true);
	selectDF(FILEID_APP,true);
	FCI fileInfo = selectEF(0x5044);

	if (fileInfo.recCount != 0x10 && fileInfo.recCount != 0x0A && fileInfo.recCount != 0x14)
	{
		//mManager.endTransaction(mConnection);
		throw CardDataError("personal data file does not contain 16 or 10 records");
	}

	data.resize(std::min(recEnd,0x10) + 1 );
	for (int i=recStart; i <= recEnd; i++)
	{
		ByteVec record = readRecord(i);
		string& ret = data[i];
		for (ByteVec::iterator c = record.begin();c!=record.end();c++)
			ret+= *c ;
		/* EstEID 3.0 has fixed length, space padded data file records. Trim from the end */
		if (getCardVersion() == VER_3_0)
			ret.erase(std::find_if(ret.rbegin(), ret.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), ret.end());
	}
	//mManager.endTransaction(mConnection);
}

bool EstEidCard::validatePin_internal(PinType pinType,PinString pin, byte &retriesLeft,bool forceUnsecure)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
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
	mManager.endTransaction(mConnection);
	return true;
}

bool EstEidCard::changePin_internal(
	PinType pinType,PinString newPin,PinString oldPin,bool useUnblockCmd ) {
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	byte cmdChangeCmd[] = {0x00,0x24,0x00};
	bool doSecure = false;

	mManager.beginTransaction(mConnection);
	if (useUnblockCmd) cmdChangeCmd[1]= 0x2C;
	ByteVec cmd(MAKEVECTOR(cmdChangeCmd));
	cmd.push_back((byte)pinType);

	size_t oldPinLen,newPinLen;
	if (newPin.length() < 4 || oldPin.length() < 4 ) {
		if (!mConnection->isSecure() )
		{
			mManager.endTransaction(mConnection);
			throw std::runtime_error("bad pin length");
		}

		doSecure = true;
	} else {
		oldPinLen = oldPin.length();
		newPinLen = newPin.length();

		ByteVec catPins;
		catPins.resize(oldPinLen + newPinLen);
		copy(oldPin.begin(), oldPin.end(), catPins.begin());
		copy(newPin.begin(), newPin.end(), catPins.begin() + oldPin.length());
		cmd.push_back(LOBYTE(catPins.size()));
		cmd.insert(cmd.end(),catPins.begin(),catPins.end());
	}

	try
	{
		if (doSecure)
			executePinChange(cmd,0,0);
		else
			execute(cmd);
	}
	catch(AuthError &ae)
	{
		mManager.endTransaction(mConnection);
		throw AuthError(ae);
	}
	catch(CardError &e)
	{
		mManager.endTransaction(mConnection);
		if (e.SW1 == 0x63)
			throw AuthError(e);
		else if (useUnblockCmd && e.SW1==0x6a && e.SW2 == 0x80 ) //unblocking, repeating old pin
			throw AuthError(e.SW1,e.SW2,true);
		else
			throw e;
	}
	mManager.endTransaction(mConnection);
	return true;
}

void EstEidCard::reconnectWithT0()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	if (mConnection && mConnection->mOwnConnection)
	{
		uint prev = mConnection->mIndex;
		delete mConnection;
		connect(prev);
	}
	else
	{
		mConnection = mManager.reconnect(mConnection);
	}
}

void EstEidCard::checkProtocol()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	//mManager.beginTransaction(mConnection);
	try
	{
		selectMF(true);
	}
	catch(CardError &ce)
	{
		//mManager.endTransaction(mConnection);
		if (ce.SW1 != 0x6A || ce.SW2 != 0x87 ) throw;
		reconnectWithT0();
	}
	catch(SCError &sce)
	{
		//mManager.endTransaction(mConnection);
		if (sce.error != 0x1      //incorrect function, some drivers do that
			&& sce.error != 0x6f7 //microsoft generic PCSC with OK3621
			) throw;
		reconnectWithT0();
	}
	//mManager.endTransaction(mConnection);
}

//transacted, public methods
string EstEidCard::readCardID()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	vector<string> temp;
	//Transaction _m(mManager,mConnection);
	//mManager.beginTransaction(mConnection);
	checkProtocol();
	readPersonalData_internal(temp,ID,ID);
	//mManager.endTransaction(mConnection);
	string ret = temp[ID];
	mManager.endTransaction(mConnection);
	return ret;
}

string EstEidCard::readDocumentID()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	vector<string> temp;
	//mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(temp,DOCUMENTID,DOCUMENTID);
	//mManager.endTransaction(mConnection);
	string ret = temp[DOCUMENTID];
	mManager.endTransaction(mConnection);
	return ret;
}	

string EstEidCard::readCardName(bool firstNameFirst)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	// FIXME: fetch something from certificate?
	if (getCardVersion() == VER_1_1)
		return "Digi ID";

	vector<string> temp;
	//Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(temp,SURNAME,FIRSTNAME);

	string ret;
	if (firstNameFirst)
		ret = temp[FIRSTNAME] + " " + temp[SURNAME];
	else
		ret = temp[SURNAME] + " " + temp[FIRSTNAME];
	mManager.endTransaction(mConnection);
	return ret;
}

void EstEidCard::reIdentify()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	byte cmdGetVersion[] = {0x00, 0xCA, 0x01, 0x00, 0x02};
	ByteVec cmd(MAKEVECTOR(cmdGetVersion));

	ByteVec result;
	try
	{
		result = execute(cmd);
	}
	catch(...)
	{
		mManager.endTransaction(mConnection);
		return;
	}
	
	if (result[0] == 0x01 && result[1] == 0x01)
		_card_version = VER_1_1;
	else if (result[0] == 0x03 && result[1] == 0x00)
		_card_version = VER_3_0;
	mManager.endTransaction(mConnection);
}

bool EstEidCard::readPersonalData(vector<string>& data, int firstRecord,int lastRecord)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	checkProtocol();
	readPersonalData_internal(data,firstRecord,lastRecord);
	mManager.endTransaction(mConnection);
	return true;
}

bool EstEidCard::getKeyUsageCounters(dword &authKey,dword &signKey)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	selectMF(true);
	selectDF(FILEID_APP,true);

	selectEF(FILEID_KEYPOINTER,true);
	ByteVec keyRec = readRecord(1);
	if (keyRec.size() != 0x15)
	{
		mManager.endTransaction(mConnection);
		throw CardDataError("key ptr len is not 0x15");
	}
	byte UsedKey = 0;
	if (keyRec[0x9]== 0x12 && keyRec[0xA] == 0x00 ) UsedKey++;

	FCI fileInfo = selectEF(0x0013);
	if (fileInfo.recCount < 4)
	{
		mManager.endTransaction(mConnection);
		throw CardDataError("key info file 0x13 should have 4 records");
	}

	ByteVec authData = readRecord(UsedKey + 3);
	ByteVec signData = readRecord(UsedKey + 1);
	ByteVec aTag = getTag(0x91,3,authData);
	ByteVec sTag = getTag(0x91,3,signData);

	authKey = 0xFFFFFF - ((aTag[0] << 16) + (aTag[1] << 8) + aTag[2]);
	signKey = 0xFFFFFF - ((sTag[0] << 16) + (sTag[1] << 8) + sTag[2]);

	mManager.endTransaction(mConnection);
	return true;
}

ByteVec EstEidCard::readEFAndTruncate(unsigned int fileLen)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	ByteVec ret = readEF(fileLen);
	if (ret.size() > 128) { //assume ASN sequence encoding with 2-byte length
		size_t realSize = ret[2] * 256 + ret[3] + 4;
		ret.resize(realSize);
	}
	return ret;
}

ByteVec EstEidCard::getAuthCert()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	ByteVec tmp;
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	selectMF(true);
	selectDF(FILEID_APP,true);
	FCI fileInfo = selectEF(0xAACE);	
	tmp = readEFAndTruncate(fileInfo.fileLength );
	mManager.endTransaction(mConnection);
	return tmp;
}

ByteVec EstEidCard::getSignCert()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	ByteVec tmp;
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);

	selectMF(true);
	selectDF(FILEID_APP,true);
	FCI fileInfo = selectEF(0xDDCE);
	tmp = readEFAndTruncate(fileInfo.fileLength );
	mManager.endTransaction(mConnection);
	return tmp;
}

ByteVec EstEidCard::calcSSL(ByteVec hash)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	ByteVec tmp = calcSign_internal(SSL,AUTH,hash,false);
	mManager.endTransaction(mConnection);
	return tmp;
}

ByteVec EstEidCard::calcSSL(ByteVec hash,PinString pin)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	//Sleep(20000);
	prepareSign_internal(AUTH,pin);
	ByteVec result = calcSign_internal(SSL,AUTH,hash,false);
	mManager.endTransaction(mConnection, false);
	return result;
	}

ByteVec EstEidCard::calcSignSHA1(ByteVec hash,KeyType keyId,bool withOID)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	//Transaction _m(mManager,mConnection);
	mManager.beginTransaction(mConnection);
	ByteVec tmp = calcSign_internal(SHA1,keyId,hash,withOID);
	mManager.endTransaction(mConnection, false);
	return tmp;
	}

ByteVec EstEidCard::calcSignSHA1(ByteVec hash,KeyType keyId,PinString pin,bool withOID)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	//Transaction _m(mManager,mConnection);
	mManager.beginTransaction(mConnection);
	prepareSign_internal(keyId,pin);
	ByteVec tmp = calcSign_internal(SHA1,keyId,hash,withOID);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

ByteVec EstEidCard::calcSignMD5(ByteVec hash,KeyType keyId,bool withOID)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	ByteVec tmp = calcSign_internal(MD5,keyId,hash,withOID);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

ByteVec EstEidCard::calcSignMD5(ByteVec hash,KeyType keyId,PinString pin,bool withOID)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);

	prepareSign_internal(keyId,pin);
	ByteVec tmp = calcSign_internal(MD5,keyId,hash,withOID);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

ByteVec EstEidCard::RSADecrypt(ByteVec cryptogram)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	ByteVec tmp = RSADecrypt_internal(cryptogram);
	mManager.endTransaction(mConnection, false);
	return tmp;
}


ByteVec EstEidCard::RSADecrypt(ByteVec cipher,PinString pin)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);

	selectMF(true);
	selectDF(FILEID_APP,true);
	enterPin(PIN_AUTH,pin);
	ByteVec tmp = RSADecrypt_internal(cipher);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::validateAuthPin(PinString pin, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	bool tmp = validatePin_internal(PIN_AUTH,pin,retriesLeft);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::validateSignPin(PinString pin, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	bool tmp = validatePin_internal(PIN_SIGN,pin,retriesLeft);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::validatePuk(PinString pin, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	bool tmp = validatePin_internal(PUK,pin,retriesLeft);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::changeAuthPin(PinString newPin,PinString oldPin, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
	{
		validatePin_internal(PIN_AUTH,oldPin,retriesLeft);
	}
	else
	{
		byte dummy;
		getRetryCounts_internal(dummy,retriesLeft,dummy);
	}
	bool tmp = changePin_internal(PIN_AUTH,newPin,oldPin);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::changeSignPin(PinString newPin,PinString oldPin, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
	{
		validatePin_internal(PIN_SIGN,oldPin,retriesLeft);
	}
	else
	{
		byte dummy;
		getRetryCounts_internal(dummy,dummy,retriesLeft);
	}
	bool tmp = changePin_internal(PIN_SIGN,newPin,oldPin);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::changePUK(PinString newPUK,PinString oldPUK, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
	{
		validatePin_internal(PUK,oldPUK,retriesLeft);
	}
	else
	{
		byte dummy;
		getRetryCounts_internal(retriesLeft,dummy,dummy);
	}
	bool tmp = changePin_internal(PUK,newPUK,oldPUK);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::unblockAuthPin(PinString newPin,PinString mPUK, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	//Transaction _m(mManager,mConnection);
	mManager.beginTransaction(mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PUK,mPUK,retriesLeft);

	for(char i='0'; i < '6'; i++)
	{
		try
		{ //ENSURE its blocked
			validatePin_internal(PIN_AUTH,PinString("0000") + i ,retriesLeft,true);
		}
		catch (...)
		{
		}
	}
	bool tmp = changePin_internal(PIN_AUTH,newPin,mPUK,true);
	mManager.endTransaction(mConnection, false);
	return tmp;
	}

bool EstEidCard::unblockSignPin(PinString newPin,PinString mPUK, byte &retriesLeft )
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	if (!mConnection->isSecure())
		validatePin_internal(PUK,mPUK,retriesLeft);
	for(char i='0'; i < '6'; i++)
	{
		try
		{ //ENSURE its blocked
			validatePin_internal(PIN_SIGN,PinString("0000") + i ,retriesLeft,true);
		}
		catch (...)
		{
		}
	}
	bool tmp = changePin_internal(PIN_SIGN,newPin,mPUK,true);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::getRetryCounts(byte &puk,byte &pinAuth,byte &pinSign)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
	//Transaction _m(mManager,mConnection);
	bool tmp = getRetryCounts_internal(puk,pinAuth,pinSign);
	mManager.endTransaction(mConnection, false);
	return tmp;
}

bool EstEidCard::getRetryCounts_internal(byte &puk,byte &pinAuth,byte &pinSign)
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	mManager.beginTransaction(mConnection);
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
	mManager.endTransaction(mConnection, false);
	return true;
}

void EstEidCard::resetAuth() {
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	try
	{
		mManager.beginTransaction(mConnection);
		//Transaction _m(mManager,mConnection);
		selectMF(true);
		setSecEnv(0);
		mManager.endTransaction(mConnection, false);
	}
	catch (...)
	{
		mManager.endTransaction(mConnection, false);
	}
}

ByteVec EstEidCard::cardChallenge()
{
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	byte cmdEntPin[] = {0x00,0x84,0x00,0x00,0x08};
	ByteVec cmd(MAKEVECTOR(cmdEntPin));
	ByteVec result;
	//mManager.beginTransaction(mConnection);
	try
	{
		result = execute(cmd);
	}
	catch(CardError e)
	{
		//mManager.endTransaction(mConnection, false);
		if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 )) 
		{
			throw AuthError(e);
		}
		throw e;
	}
	//mManager.endTransaction(mConnection, false);
	return result;
}

ByteVec EstEidCard::runCommand( ByteVec vec ) {
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	ByteVec result;

	int retry = 3;
	
	while( retry )
	{
		mManager.beginTransaction(mConnection);
		try
		{
			result = execute(vec);
			break;
		}
		catch(CardError e)
		{
			mManager.endTransaction(mConnection, false);
			if ( retry == 1 )
			{
				if (e.SW1 == 0x69 && (e.SW2 == 0x82 || e.SW2 == 0x00 || e.SW2 == 0x85 )) 
				{
					mManager.endTransaction(mConnection, false);
					throw AuthError(e);
				}
				throw e;
			}
			else
			{
				reconnectWithT0();
			}
		}
		mManager.endTransaction(mConnection, false);
		retry--;
	}

	return result;
}

std::string EstEidCard::getMD5KeyContainerName(int type)
{
	std::string keyContainerNameMD5 = "";
#ifdef WIN32
	mManager.writeLog("[%s:%d]", __FUNCTION__, __LINE__);
	std::string keyContainerName = "";
	
	try
	{
		keyContainerName.append(readDocumentID());
		keyContainerName.append("_");
		switch(type)
		{
			case 0: keyContainerName.append("AUT"); break;
			case 1: keyContainerName.append("SIG"); break;
			default: throw std::runtime_error("Invalid key container type.");
		}
		
		keyContainerNameMD5 = md5(keyContainerName);

		mManager.writeLog("[%s:%d] Returning container name MD5: %s", __FUNCTION__, __LINE__, keyContainerNameMD5.c_str());
	}
	catch(CardError &e)
	{
		mManager.writeLog("[%s:%d] Card error: %s, 0x%X, 0x%X", __FUNCTION__, __LINE__, e.what(), e.SW1, e.SW2);
		throw e;
	}
	catch(std::runtime_error &re)
	{
		mManager.writeLog("[%s:%d] Runtime error: %s", __FUNCTION__, __LINE__, re.what());
		throw re;
	}

	
#endif
	return keyContainerNameMD5;
}
