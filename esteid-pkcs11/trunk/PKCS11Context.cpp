#include "precompiled.h"
#include "PKCS11Context.h"
#include <smartcardpp/PCSCManager.h>
#include <smartcardpp/helperMacro.h>
#include <smartcardpp/esteid/EstEidCard.h>
#include <utility/asnCertificate.h>
#include <utility/logger.h>

#include <string.h>

#undef min

enum ObjID {
	OBJ_INVALID = -1,
	OBJ_NONE,
	OBJ_CERT,
	OBJ_PRIVKEY,
	OBJ_PUBKEY
} ;

class PKCS11Object;
class searchTerm;

typedef std::vector<PKCS11Object >::iterator objectIter;
typedef std::vector<CK_ATTRIBUTE >::iterator attrIter;

class ObjHandle {
	CK_OBJECT_HANDLE h;
public:
	ObjHandle &operator=(const PKCS11Object &obj);
	ObjHandle(const ObjID &handle) : h(handle) {}
	ObjHandle(const CK_OBJECT_HANDLE &handle) : h(handle) {}
	ObjHandle() : h(OBJ_INVALID) {}
	bool operator==(const ObjID& other) {
		return h == other;
		}
	bool operator==(const ObjHandle &other) {return h == other.h ; }
	};

class PKCS11Object {
protected:
	ObjHandle handle;
	std::vector<CK_ATTRIBUTE> attrib;
	friend class ObjHandle;
	friend class searchTerm;
public:
	PKCS11Object(ObjID id,CK_ATTRIBUTE *att,size_t count) : 
		handle(id),attrib(att,att + count) {}
	bool operator==(const ObjHandle &objHandle) {
		return handle == objHandle;
		}
	attrIter findAttrib(CK_ATTRIBUTE_PTR att) {
		return find(attrib.begin(),attrib.end(),att->type);
		}
	void appendAttrib(CK_ATTRIBUTE att) {
		attrib.push_back(att);
		}
	bool noAttrib(attrIter &ref) {return ref == attrib.end();}
	};

ObjHandle & ObjHandle::operator=(const PKCS11Object &obj) {
	h = obj.handle.h;
	return *this;
	}

CK_OBJECT_CLASS pubkey_class = CKO_PUBLIC_KEY;
CK_OBJECT_CLASS privkey_class = CKO_PRIVATE_KEY;
CK_OBJECT_CLASS cert_class = CKO_CERTIFICATE;
CK_CERTIFICATE_TYPE cert_type = CKC_X_509;
CK_BBOOL _true = TRUE;
CK_KEY_TYPE keyType = CKK_RSA;
CK_BYTE modulus[128] = {1};
CK_ULONG modBits = 1024;
CK_ATTRIBUTE publicKeyTemplate[] = {
	{CKA_CLASS, &pubkey_class, sizeof(pubkey_class)},
	{CKA_TOKEN, &_true, sizeof(_true)},
	{CKA_KEY_TYPE,&keyType,sizeof(keyType)},
	{CKA_ENCRYPT, &_true, sizeof(_true)},
	{CKA_MODULUS,modulus,sizeof(modulus)},
	{CKA_MODULUS_BITS,&modBits,sizeof(modBits)},
};
CK_ATTRIBUTE privateKeyTemplate[] = {
	{CKA_CLASS, &privkey_class, sizeof(privkey_class)},
	{CKA_TOKEN, &_true, sizeof(_true)},
	{CKA_KEY_TYPE,&keyType,sizeof(keyType)},
	{CKA_PRIVATE,&_true,sizeof(_true)},
	{CKA_MODULUS,modulus,sizeof(modulus)},
	{CKA_SIGN,&_true,sizeof(_true)}
};
CK_ATTRIBUTE certificateTemplate[] = {
	{CKA_CLASS, &cert_class, sizeof(cert_class)},
	{CKA_TOKEN, &_true, sizeof(_true)},
	{CKA_CERTIFICATE_TYPE, &cert_type, sizeof(cert_type)},
	{CKA_TRUSTED,&_true,sizeof(_true)},
};

class PKCS11Session {
public:
	struct utf8str : public std::vector<CK_UTF8CHAR> {
		utf8str &operator=(const std::string _in) {
			resize(_in.length());
			std::copy(_in.begin(),_in.end(),begin());
			return *this;
			}
	};
private:
	friend class PKCS11Context;
//	friend struct SessionChangeState;
	CK_SESSION_HANDLE session;
	CK_SLOT_ID slotID;
	int readerID;
	CK_FLAGS flags;
	CK_VOID_PTR pApplication;
	CK_NOTIFY   notify;
	std::vector<CK_ATTRIBUTE > searchParam;
	std::vector<ObjHandle> searchHandles;
	std::vector<PKCS11Object > objects;
	struct certFields {
		CK_BYTE id;
		ByteVec cert;
		ByteVec pubKey;
		ByteVec iss;
		ByteVec ser;
		ByteVec sub;
		utf8str label;
		utf8str pubKeyLabel;
		utf8str privKeyLabel;
	} auth,sign;
	PinString pin;
	CK_ULONG state;
	CK_MECHANISM   sigMechanism;
	CK_OBJECT_HANDLE  sigKey;
public:
	struct changeState {
		CK_ULONG mState;
		CK_SLOT_ID mSlot;
		PinString mPin;
		changeState(CK_SLOT_ID slot, CK_ULONG state,PinString pin)
			: mSlot(slot), mState(state), mPin(pin) {}
		void operator() (PKCS11Session &obj) {
			if (obj.slotID == mSlot) {
				obj.state = mState;
				obj.pin = mPin;
				}
		}
	};
	PKCS11Session(CK_SESSION_HANDLE ,CK_SLOT_ID , CK_FLAGS,CK_VOID_PTR,CK_NOTIFY);
	~PKCS11Session();
	bool operator==(const CK_SESSION_HANDLE& other) {
		return session == other;
		}
	void createCertificate(ByteVec certBlob,certFields & _a,std::string label,CK_BYTE id) {
		_a.cert = certBlob;
		std::stringstream dummy;
		asnCertificate asnCert(_a.cert,dummy);
		_a.id = id;
		_a.pubKey = asnCert.getPubKey();
		_a.iss = asnCert.getIssuerBlob();
		_a.ser = asnCert.getSerialBlob();
		_a.sub = asnCert.getSubjectBlob();
		_a.label = label + " certificate";
		_a.pubKeyLabel = label + " public key";
		_a.privKeyLabel = label + " private key";

		CK_ATTRIBUTE valId = {CKA_ID, &_a.id, sizeof(_a.id)};
		CK_ATTRIBUTE valAttCert= {CKA_VALUE,&_a.cert[0],(CK_ULONG)_a.cert.size()};
		CK_ATTRIBUTE valAttPubKey = {CKA_VALUE,&_a.pubKey[0],(CK_ULONG)_a.pubKey.size()};
		CK_ATTRIBUTE valIssuer = {CKA_ISSUER,&_a.iss[0],(CK_ULONG) _a.iss.size()};
		CK_ATTRIBUTE valSerial = {CKA_SERIAL_NUMBER,&_a.ser[0],(CK_ULONG) _a.ser.size()};
		CK_ATTRIBUTE valSubject = {CKA_SUBJECT,&_a.sub[0],(CK_ULONG) _a.sub.size()};
		CK_ATTRIBUTE valLabel = {CKA_LABEL,&_a.label[0],(CK_ULONG)_a.label.size()};
		CK_ATTRIBUTE valLabel0 = {CKA_LABEL,&_a.pubKeyLabel[0],(CK_ULONG)_a.pubKeyLabel.size()};
		CK_ATTRIBUTE valLabel1 = {CKA_LABEL,&_a.privKeyLabel[0],(CK_ULONG)_a.privKeyLabel.size()};

		objects.push_back(PKCS11Object(OBJ_CERT,certificateTemplate,LENOF(certificateTemplate)));
		(--objects.end())->appendAttrib(valId);
		(--objects.end())->appendAttrib(valAttCert);
		(--objects.end())->appendAttrib(valIssuer);
		(--objects.end())->appendAttrib(valSerial);
		(--objects.end())->appendAttrib(valSubject);
		(--objects.end())->appendAttrib(valLabel);
		objects.push_back(PKCS11Object(OBJ_PUBKEY,publicKeyTemplate,LENOF(publicKeyTemplate)));
		(--objects.end())->appendAttrib(valId);
		(--objects.end())->appendAttrib(valAttPubKey);
		(--objects.end())->appendAttrib(valLabel0);
		objects.push_back(PKCS11Object(OBJ_PRIVKEY,privateKeyTemplate,LENOF(privateKeyTemplate)));
		(--objects.end())->appendAttrib(valId);
		(--objects.end())->appendAttrib(valLabel1);
		}
};

bool operator==(CK_ATTRIBUTE x,CK_ATTRIBUTE_TYPE a) {
	return x.type == a;
	}

class PKCS11ContextPriv {
	logger m_log;

	friend class PKCS11Context;
	PCSCManager mgr;
	uint readerCount;
	CK_SESSION_HANDLE nextSession;
	std::vector<PKCS11Session > sessions;
	typedef std::vector<PKCS11Session >::iterator sessIter;
	PKCS11ContextPriv() : nextSession(303),m_log("EstEidPKCS11") {}
	bool checkSlot(CK_SLOT_ID slotID) {
		return (slotID >= (readerCount * 2));
		}
};


PKCS11Context::PKCS11Context(void)
{
	d = new PKCS11ContextPriv();
}

PKCS11Context::~PKCS11Context(void)
{
	delete d;
}

PKCS11Session::	PKCS11Session(CK_SESSION_HANDLE sh,CK_SLOT_ID s, 
	  CK_FLAGS f,CK_VOID_PTR app,CK_NOTIFY n) :
	session(sh),slotID(s),flags(f),pApplication(app),notify(n),
	state(CKS_RO_PUBLIC_SESSION)
{
}

PKCS11Session::~PKCS11Session(void) 
{
}

void PKCS11Context::refreshReaders() {
	d->readerCount = d->mgr.getReaderCount();
	}

CK_SESSION_HANDLE PKCS11Context::getNextSessionHandle() {
	return ++d->nextSession;
	}

void padString(CK_UTF8CHAR *s,int len,std::string rstr) {
	memset(s,' ',len);
	memcpy(s,rstr.c_str(),std::min((int)rstr.length(),len));
	}

#define F_LOG(a) ScopedLog _l(#a,d->m_log)

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetInfo(CK_INFO_PTR   pInfo  )) {
	F_LOG(C_GetInfo);

	d->m_log << "begin" << std::endl;

	pInfo->cryptokiVersion.major = 2;
	pInfo->cryptokiVersion.minor = 11;
	padString(pInfo->manufacturerID, sizeof(pInfo->manufacturerID)
		,"EstEID ( pkcs11 opensource )");
	pInfo->flags = 0;
	padString(pInfo->libraryDescription,sizeof(pInfo->libraryDescription)
		,"EstEID PKCS#11 Library");
	pInfo->libraryVersion.major = 0;
	pInfo->libraryVersion.minor = 1;
	return CKR_OK;
	}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetTokenInfo(
	CK_SLOT_ID        slotID,  /* ID of the token's slot */
	CK_TOKEN_INFO_PTR pInfo    /* receives the token information */
	)) {
	F_LOG(C_GetTokenInfo);

	try {
		refreshReaders();
		if (d->checkSlot(slotID)) return CKR_SLOT_ID_INVALID;
		int readerId = slotID / 2;
		EstEidCard card(d->mgr);
		if (!card.isInReader(readerId)) return CKR_DEVICE_REMOVED;
		card.connect(readerId);
		std::string id = card.readCardID();
		std::string name = card.readCardName();
		if (slotID & 1)
		  name += " (PIN2, Sign)";
		else
		  name += " (PIN1, Auth)";

		memset(pInfo, 0, sizeof(*pInfo));
		padString(pInfo->label,sizeof(pInfo->label),name);
		padString(pInfo->manufacturerID, sizeof(pInfo->manufacturerID), "EstEid");
		padString(pInfo->model, sizeof(pInfo->model), "original");
		padString(pInfo->serialNumber, sizeof(pInfo->serialNumber), id);
		pInfo->flags = CKF_WRITE_PROTECTED |CKF_LOGIN_REQUIRED | CKF_USER_PIN_INITIALIZED|
			CKF_TOKEN_INITIALIZED |0;

		pInfo->ulMaxSessionCount = 10;     /* max open sessions */
		pInfo->ulSessionCount = 0;        /* sess. now open */
		pInfo->ulMaxRwSessionCount = 10;   /* max R/W sessions */
		pInfo->ulRwSessionCount = 0;      /* R/W sess. now open */
		pInfo->ulMaxPinLen = 12;           /* in bytes */
		pInfo->ulMinPinLen = 4;           /* in bytes */
		pInfo->ulTotalPublicMemory = 1024;   /* in bytes */
		pInfo->ulFreePublicMemory  = 0;    /* in bytes */
		pInfo->ulTotalPrivateMemory = 1024;  /* in bytes */
		pInfo->ulFreePrivateMemory = 0;   /* in bytes */

		CK_VERSION nulVer = {1,0};
		pInfo->hardwareVersion = nulVer;
		pInfo->firmwareVersion = nulVer;
		return CKR_OK;
	}catch(std::runtime_error &err) {
		return CKR_GENERAL_ERROR;
		}
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetSlotList(
		CK_BBOOL       tokenPresent,  /* only slots with tokens? */
		CK_SLOT_ID_PTR pSlotList,     /* receives array of slot IDs */
		CK_ULONG_PTR   pulCount       /* receives number of slots */
		)) {
	F_LOG(C_GetSlotList);

	try {
		uint inBuffer = *pulCount;
		*pulCount = 0;
		refreshReaders();
		*pulCount = d->readerCount * 2;

		if (pSlotList == NULL )
			return CKR_OK;
		if (d->readerCount > inBuffer) 
			return CKR_BUFFER_TOO_SMALL;

		CK_SLOT_ID_PTR pSlot = pSlotList;
		for(uint i = 0; i < d->readerCount ; i++ ) {
			EstEidCard card(d->mgr);
			if (!tokenPresent || card.isInReader(i)) {
				*pSlot++ = (i * 2);
				*pSlot++ = (i * 2) + 1;
				}
			}

		return CKR_OK;
	}catch(std::runtime_error &) {
		return CKR_GENERAL_ERROR;
		}
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetSlotInfo(
	CK_SLOT_ID        slotID,  /* ID of the token's slot */
	CK_SLOT_INFO_PTR pInfo    /* receives the slot information */
	)) {
	F_LOG(C_GetSlotInfo);

	try {
		refreshReaders();
		if (d->checkSlot(slotID)) return CKR_SLOT_ID_INVALID;
		EstEidCard card(d->mgr);
		
		memset(pInfo, 0, sizeof(*pInfo));
		std::ostringstream buf;
		buf << "EstSlot " << slotID;
		padString(pInfo->slotDescription,sizeof(pInfo->slotDescription), buf.str()) ;  
		padString(pInfo->manufacturerID,sizeof(pInfo->manufacturerID),"EstEID");
		pInfo->flags = CKF_REMOVABLE_DEVICE | CKF_HW_SLOT;
#ifdef ONE_KEY
		if (!(slotID & 1))
#endif
		  if (card.isInReader(slotID / 2)) pInfo->flags |= CKF_TOKEN_PRESENT;

		CK_VERSION nulVer = {1,0};
		pInfo->hardwareVersion = nulVer;
		pInfo->firmwareVersion = nulVer;
		return CKR_OK;
	}catch(std::runtime_error &) {
		return CKR_GENERAL_ERROR;
		}
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetMechanismList(
	CK_SLOT_ID            slotID,          /* ID of token's slot */
	CK_MECHANISM_TYPE_PTR pMechanismList,  /* gets mech. array */
	CK_ULONG_PTR          pulCount         /* gets # of mechs. */
	)) {
	F_LOG(C_GetMechanismList);

	CK_ULONG count = *pulCount;
//	refreshReaders();
	if (d->checkSlot(slotID)) return CKR_SLOT_ID_INVALID;
	*pulCount = 1;
	if (pMechanismList == NULL )
		return CKR_OK;
	if (count < 1 ) 
		return CKR_BUFFER_TOO_SMALL;
	pMechanismList[0] = CKM_RSA_PKCS;
	/*pMechanismList[1] = CKM_MD5_RSA_PKCS;
	pMechanismList[2] = CKM_SHA1_RSA_PKCS;*/
	return CKR_OK;
	}

/*
CK_ULONG    ulMinKeySize;
    CK_ULONG    ulMaxKeySize;
    CK_FLAGS    flags;
*/
CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetMechanismInfo(
		CK_SLOT_ID            slotID,  /* ID of the token's slot */
		CK_MECHANISM_TYPE     type,    /* type of mechanism */
		CK_MECHANISM_INFO_PTR pInfo    /* receives mechanism info */
		)) {
	F_LOG(C_GetMechanismInfo);

	pInfo->ulMinKeySize = 1024;
	pInfo->ulMaxKeySize = 1024;
	pInfo->flags = CKF_HW | CKF_SIGN /*| CKF_VERIFY*/;
	return CKR_OK;
	}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_InitToken(
		CK_SLOT_ID      slotID,    /* ID of the token's slot */
		CK_UTF8CHAR_PTR pPin,      /* the SO's initial PIN */
		CK_ULONG        ulPinLen,  /* length in bytes of the PIN */
		CK_UTF8CHAR_PTR pLabel     /* 32-byte token label (blank padded) */
		)) {
	F_LOG(C_InitToken);

	return CKR_OK;
	}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_OpenSession(
	CK_SLOT_ID            slotID,        /* the slot's ID */
	CK_FLAGS              flags,         /* from CK_SESSION_INFO */
	CK_VOID_PTR           pApplication,  /* passed to callback */
	CK_NOTIFY             Notify,        /* callback function */
	CK_SESSION_HANDLE_PTR phSession      /* gets session handle */
	)) {
	F_LOG(C_OpenSession);

	try {
		refreshReaders();
		if (d->checkSlot(slotID)) return CKR_SLOT_ID_INVALID;
		if (!(flags & CKF_SERIAL_SESSION)) return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
		
		*phSession = getNextSessionHandle();
		d->sessions.push_back(PKCS11Session(*phSession,slotID,
			flags,pApplication,Notify
			));
		PKCS11ContextPriv::sessIter sessI,sess = --d->sessions.end();
		for(sessI = d->sessions.begin() ; sessI != d->sessions.end() ; sessI++ ) {
			if (sessI->slotID == slotID ) {
				sess->state = sessI->state;
				sess->pin = sessI->pin;
				break;
				}
			}
		sess->readerID = slotID / 2;
		{
			EstEidCard card(d->mgr);
			if (!card.isInReader(sess->readerID)) 
				return CKR_TOKEN_NOT_PRESENT;
			card.connect(sess->readerID);
			if (slotID & 0x1) {
//				sess->createCertificate(card.getSignCert(),sess->sign,"Signature",223);
			} else
				sess->createCertificate(card.getAuthCert(),sess->auth,"Authentication",123);
		}
		return CKR_OK;
	}catch(std::runtime_error &) {
		return CKR_GENERAL_ERROR;
		}
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetSessionInfo(
		CK_SESSION_HANDLE   hSession, 
		CK_SESSION_INFO_PTR pInfo
		)) {
	F_LOG(C_GetSessionInfo);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;

	pInfo->slotID = sess->slotID ;
	pInfo->state = sess->state;
	pInfo->flags = sess->flags;
	pInfo->ulDeviceError = 0;
	return CKR_OK;
	}

class searchTerm {
	std::vector<CK_ATTRIBUTE > param;
	CK_ULONG mState; //public/private
	friend class PKCS11Object;
public:
	searchTerm(std::vector<CK_ATTRIBUTE > searchParam,CK_ULONG state) : 
	  param(searchParam),mState(state) {}
	bool operator() (const PKCS11Object& value) {
		std::vector<CK_ATTRIBUTE >::const_iterator objAtt;
		if (mState == CKS_RO_PUBLIC_SESSION) {//check if CK_PRIVATE is set
			objAtt  =  find(value.attrib.begin(),value.attrib.end(),CKA_PRIVATE);
			if (objAtt != value.attrib.end() ) //private was found
				return true;
			}
		for(attrIter att = param.begin(); att != param.end(); att++) {
			objAtt = find(value.attrib.begin(),value.attrib.end(),att->type);
			if (value.attrib.end() == objAtt) //object does not have required attribute
				return true;
			void * test=objAtt->pValue;
			void * test2=att->pValue;
			if (memcmp(objAtt->pValue,att->pValue,std::min(objAtt->ulValueLen,att->ulValueLen ))) 
				return true; //has attribute but does not match
			}
		return false;
		}
	bool operator() (const ObjHandle& value) {
		return true;
		}
};

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_FindObjectsInit(
	CK_SESSION_HANDLE hSession,   /* the session's handle */
	CK_ATTRIBUTE_PTR  pTemplate,  /* attribute values to match */
	CK_ULONG          ulCount     /* attrs in search template */
	)) {
	F_LOG(C_FindObjectsInit);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	sess->searchParam.clear();

	for(uint i = 0; i < ulCount ; i++ )
		sess->searchParam.push_back(*(pTemplate + i));

	sess->searchHandles.clear();
	sess->searchHandles.resize(sess->objects.size());
	std::vector<ObjHandle >::iterator copy = 
		remove_copy_if(sess->objects.begin(),sess->objects.end(),
		sess->searchHandles.begin(),searchTerm(sess->searchParam,sess->state));
	sess->searchHandles.resize(copy - sess->searchHandles.begin());

	return CKR_OK;
	}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_FindObjects(
	CK_SESSION_HANDLE    hSession,          /* session's handle */
	CK_OBJECT_HANDLE_PTR phObject,          /* gets obj. handles */
	CK_ULONG             ulMaxObjectCount,  /* max handles to get */
	CK_ULONG_PTR         pulObjectCount     /* actual # returned */
	)) {
	F_LOG(C_FindObjects);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	
	CK_ULONG returnCount = std::min(ulMaxObjectCount, (CK_ULONG)sess->searchHandles.size());
	if (returnCount)
		memcpy(phObject,&sess->searchHandles[0],returnCount * sizeof(*phObject));
	*pulObjectCount = returnCount;
	sess->searchHandles.erase(sess->searchHandles.begin(),sess->searchHandles.begin() + returnCount);
	return CKR_OK;
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_FindObjectsFinal(
	CK_SESSION_HANDLE hSession  /* the session's handle */
	)) {
	F_LOG(C_FindObjectsFinal);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	sess->searchParam.clear();
	sess->searchHandles.clear();
	return CKR_OK;
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_Login(
	CK_SESSION_HANDLE hSession,  /* the session's handle */
	CK_USER_TYPE      userType,  /* the user type */
	CK_UTF8CHAR_PTR   pPin,      /* the user's PIN */
	CK_ULONG          ulPinLen   /* the length of the PIN */
	)) {
	F_LOG(C_Login);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;

	sess->pin = PinString((const char *) pPin,(size_t)ulPinLen);

	try {
		EstEidCard card(d->mgr);
		if (!card.isInReader(sess->readerID)) 
			return CKR_DEVICE_REMOVED;
		card.connect(sess->readerID);
		byte retriesLeft;
		if (sess->slotID & 1)
		  card.validateSignPin(sess->pin,retriesLeft);
		else
		  card.validateAuthPin(sess->pin,retriesLeft);
	} catch ( AuthError &ae) {
		if (ae.m_badinput) 
			return CKR_PIN_LEN_RANGE;
		else
			return CKR_PIN_INCORRECT;
	} catch( std::runtime_error &err) {
		return CKR_GENERAL_ERROR;
		}

	for_each(d->sessions.begin(),d->sessions.end(),
		PKCS11Session::changeState(sess->slotID,CKS_RO_USER_FUNCTIONS,sess->pin));
	return CKR_OK;
	}

CK_DEFINE_FUNCTION(CK_RV,PKCS11Context::C_Logout(
	   CK_SESSION_HANDLE hSession  /* the session's handle */ )) {
	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	for_each(d->sessions.begin(),d->sessions.end(),
		PKCS11Session::changeState(sess->slotID,CKS_RO_USER_FUNCTIONS,sess->pin));
	sess->pin.clear();
	return CKR_OK;
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_GetAttributeValue(
		CK_SESSION_HANDLE hSession,   /* the session's handle */
		CK_OBJECT_HANDLE  hObject,    /* the object's handle */
		CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attrs; gets vals */
		CK_ULONG          ulCount     /* attributes in template */
		)) {
	F_LOG(C_GetAttributeValue);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	objectIter obj = find(sess->objects.begin(),sess->objects.end(),
		ObjHandle(hObject));
	if (obj == sess->objects.end()) 
		return CKR_OBJECT_HANDLE_INVALID;
	for(uint i = 0; i < ulCount ; i++) {
		CK_ATTRIBUTE_PTR attrib = pTemplate + i;
		attrIter objAttrib = obj->findAttrib(attrib);
		attrib->ulValueLen = -1;
		if (obj->noAttrib(objAttrib)) 
			continue;
		if (attrib->pValue == NULL) {
			attrib->ulValueLen = objAttrib->ulValueLen;
			continue;
			}
		if (attrib->ulValueLen >= objAttrib->ulValueLen) {
			memcpy(attrib->pValue,objAttrib->pValue,std::min(attrib->ulValueLen,objAttrib->ulValueLen));
			attrib->ulValueLen = objAttrib->ulValueLen;
			continue;
			}
		}
	return CKR_OK;
	}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_SignInit(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_MECHANISM_PTR  pMechanism,  /* the signature mechanism */
	CK_OBJECT_HANDLE  hKey )) {    /* handle of signature key */
	F_LOG(C_SignInit);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	if (sess->state != CKS_RO_USER_FUNCTIONS) 
		return CKR_USER_NOT_LOGGED_IN;

	sess->sigMechanism = *pMechanism;
	sess->sigKey = hKey;
	switch(sess->sigMechanism.mechanism) {
		case CKM_RSA_PKCS:
		  if (sess->slotID & 1) return CKR_MECHANISM_INVALID;
//		case CKM_MD5_RSA_PKCS:
//		case CKM_SHA1_RSA_PKCS: 
		  break;
		default: return CKR_MECHANISM_INVALID;
		}
	return CKR_OK;
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_Sign(
	CK_SESSION_HANDLE hSession,        /* the session's handle */
	CK_BYTE_PTR       pData,           /* the data to sign */
	CK_ULONG          ulDataLen,       /* count of bytes to sign */
	CK_BYTE_PTR       pSignature,      /* gets the signature */
	CK_ULONG_PTR      pulSignatureLen )) {/* gets signature length */
	F_LOG(C_Sign);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	if (sess->state != CKS_RO_USER_FUNCTIONS) 
		return CKR_USER_NOT_LOGGED_IN;
	try {
		CK_ULONG len = *pulSignatureLen;
		refreshReaders();
		EstEidCard card(d->mgr);
		if (!card.isInReader(sess->readerID)) 
			return CKR_DEVICE_REMOVED;
		card.connect(sess->readerID);

		ByteVec input(pData, pData + ulDataLen), result;
		EstEidCard::KeyType key = sess->slotID & 0x1 ? EstEidCard::SIGN : EstEidCard::AUTH;
		switch(sess->sigMechanism.mechanism) {
			case CKM_RSA_PKCS:
			    result = card.calcSSL(input,sess->pin);
			    break;
			case CKM_SHA1_RSA_PKCS:
			    result = card.calcSignSHA1(input,key,sess->pin);
			break;
			}

		*pulSignatureLen = (CK_ULONG)result.size();
		if (len < result.size() ) 
			return CKR_BUFFER_TOO_SMALL;
		if (pSignature == NULL)
			return CKR_OK;
		memcpy(pSignature,&result[0],result.size());
		return CKR_OK;
	} catch ( AuthError &ae) {
		if (ae.m_badinput) 
			return CKR_PIN_LEN_RANGE;
		else
			return CKR_PIN_INCORRECT;
	}catch(CardError &ce) {
		std::ostringstream buf;
		buf << ce.what();
		return CKR_FUNCTION_FAILED;
	}catch(std::runtime_error &err) {
		std::ostringstream buf;
		buf << err.what();
		return CKR_GENERAL_ERROR;
		}
	return CKR_OK;
}

CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_SignUpdate(
		CK_SESSION_HANDLE hSession,  /* the session's handle */
		CK_BYTE_PTR       pPart,     /* the data to sign */
		CK_ULONG          ulPartLen )) {/* count of bytes to sign */
	F_LOG(C_SignUpdate);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	if (sess->state != CKS_RO_USER_FUNCTIONS) 
		return CKR_USER_NOT_LOGGED_IN;

	return CKR_FUNCTION_NOT_SUPPORTED;
}
CK_DECLARE_FUNCTION(CK_RV,PKCS11Context::C_SignFinal(
		CK_SESSION_HANDLE hSession,        /* the session's handle */
		CK_BYTE_PTR       pSignature,      /* gets the signature */
		CK_ULONG_PTR      pulSignatureLen )) { /* gets signature length */
	F_LOG(C_SignFinal);

	PKCS11ContextPriv::sessIter sess = find(d->sessions.begin(),d->sessions.end(),hSession);
	if (d->sessions.end() == sess)
		return CKR_SESSION_HANDLE_INVALID;
	if (sess->state != CKS_RO_USER_FUNCTIONS) 
		return CKR_USER_NOT_LOGGED_IN;

	return CKR_FUNCTION_NOT_SUPPORTED;
}

