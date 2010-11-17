/*!
	\file		EstEidCard.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2010-02-04 08:10:53 +0200 (N, 04 veebr 2010) $
*/
// Revision $Revision: 512 $
#pragma once
#include "../CardBase.h"
#include "../PinString.h"

/// Estonian ID card class. Supplies most of the card functions
class EstEidCard:
	public CardBase
{
	friend class EstEidCardMaintainer;
protected:
	enum {
		FILEID_MASTER = 0x3F00,
		FILEID_APP	  =	0xEEEE,
		FILEID_RETRYCT =	0x0016,
		FILEID_KEYPOINTER  = 0x0033
	};
public:

	enum PinType {
		PUK = 0,
		PIN_AUTH = 1,
		PIN_SIGN = 2
		};
	enum KeyType { AUTH = 0,SIGN = 1 };
	enum AlgType {
		MD5,SHA1,SSL
		};
	enum RecordNames {
		SURNAME = 1,
		FIRSTNAME,
		MIDDLENAME,
		SEX,
		CITIZEN,
		BIRTHDATE,
		ID,
		DOCUMENTID,
		EXPIRY,
		BIRTHPLACE,
		ISSUEDATE,
		RESIDENCEPERMIT,
		COMMENT1,COMMENT2,COMMENT3,COMMENT4
		};
private:
	void prepareSign_internal(KeyType keyId,PinString pin);
	ByteVec calcSign_internal(AlgType type,KeyType keyId, ByteVec hash,bool withOID = true);
	ByteVec RSADecrypt_internal(ByteVec cipher);
	void readPersonalData_internal(vector<std::string>& data,
		int firstRecord,int lastRecord );
	bool validatePin_internal(PinType pinType,PinString pin, byte &retriesLeft,
		bool forceUnsecure = false);
	bool changePin_internal(
		PinType pinType,PinString newPin,PinString oldPin,bool useUnblockCommand=false);
	void checkProtocol();
	bool getRetryCounts_internal(byte &puk,byte &pinAuth,byte &pinSign);
	ByteVec readEFAndTruncate(unsigned int fileLen);

public:
	EstEidCard(ManagerInterface &ref) : CardBase(ref) {}
	EstEidCard(ManagerInterface &ref,unsigned int idx) :
	  CardBase(ref,idx) {}
	EstEidCard(ManagerInterface &ref,ConnectionBase *conn)  :
	  CardBase(ref,conn) {}
	~EstEidCard() {}
	bool isInReader(unsigned int idx);
	bool isSecureConnection();
	void reconnectWithT0();

	void enterPin(PinType pinType,PinString pin,bool forceUnsecure = false);

	/// Reads the card holder identification code from personal data file
	std::string readCardID();
	/// Reads the card holder card number from personal data file
	std::string readDocumentID();
	/// Reads the card holder name from personal data file, bool flag flips display order
	std::string readCardName(bool firstNameFirst = false);
	/// Reads entire or parts of personal data file from firstRecord to LastRecord
	bool readPersonalData(std::vector<std::string>& data,
		int firstRecord=SURNAME,int lastRecord=EXPIRY);
	/// gets accumulated key usage counters from the card
	bool getKeyUsageCounters(dword &authKey,dword &signKey);
	/// gets PIN entry retry counts for all three PINs
	bool getRetryCounts(byte &puk,byte &pinAuth,byte &pinSign);
	/// retrieve Authentication certificate
	ByteVec getAuthCert();
	/// retrieve Signature certificate
	ByteVec getSignCert();
	/// calculate SSL signature for SHA1+MD5 hash. PIN needs to be entered before
	ByteVec calcSSL(ByteVec hash);
	/// calculate SSL signature with PIN supplied, supply empty pin if cardmanager supports pin entry
	ByteVec calcSSL(ByteVec hash,PinString pin);

	/// calculate signature over SHA1 hash, keyid =0 selects auhtentication key, other values signature key. withOID=false calculates without SHA1 signatures, used for VPN
	ByteVec calcSignSHA1(ByteVec hash,KeyType keyId,bool withOID = true);
	/// calculate SHA1 signature with pin
	ByteVec calcSignSHA1(ByteVec hash,KeyType keyId,PinString pin,bool withOID = true);

	/// calculate signature over MD5 hash, keyid =0 selects auhtentication key
	ByteVec calcSignMD5(ByteVec hash,KeyType keyId,bool withOID = true);
	/// calculate signature over MD5 hash, with pin
	ByteVec calcSignMD5(ByteVec hash,KeyType keyId,PinString pin,bool withOID = true);

	/// decrypt RSA bytes, from 1024 bit/128 byte input vector, using authentication key
	ByteVec RSADecrypt(ByteVec cipher);
	/// decrypt RSA with authentication key, with pin supplied
	ByteVec RSADecrypt(ByteVec cipher,PinString pin);

	/// enter and validate authentication PIN. AuthError will be thrown if invalid
	bool validateAuthPin(PinString pin,byte &retriesLeft );
	/// enter and validate signature PIN
	bool validateSignPin(PinString pin,byte &retriesLeft );
	/// enter and validate PUK code
	bool validatePuk(PinString puk, byte &retriesLeft );

	/// change authentication PIN. For secure pin entry, specify pin lengths in "04" format, i.e. two-byte decimal string
	bool changeAuthPin(PinString newPin,PinString oldPin, byte &retriesLeft );
	/// change signature PIN
	bool changeSignPin(PinString newPin,PinString oldPin, byte &retriesLeft );
	/// change PUK
	bool changePUK(PinString newPUK,PinString oldPUK, byte &retriesLeft );
	/// unblock signature PIN using PUK. if correct PUK is supplied, the PIN will be first blocked and then unblocked
	bool unblockAuthPin(PinString newPin,PinString PUK, byte &retriesLeft );
	/// unblock signature PIN
	bool unblockSignPin(PinString newPin,PinString PUK, byte &retriesLeft );

	/// set security environment for the card. This does not need to be called directly, normally
	void setSecEnv(byte env);
	/// reset authentication, so next crypto operations will require new pin entry
	void resetAuth();

	//get random challenge number
	ByteVec cardChallenge();
	//execute commands
	ByteVec runCommand( ByteVec vec );
};
