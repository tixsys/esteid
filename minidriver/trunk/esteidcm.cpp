// esteidcm.cpp : Defines the entry point for the DLL application.
//

#include "precompiled.h"
#include "esteidcm.h"

#include "smartcardpp/esteid/EstEidCard.h"
#include "smartcardpp/PCSCManager.h"
#include "smartcardpp/SCError.h"
#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <string>
#include <time.h>
#include "nullstream.h"

using std::wstring;
using std::string;
using std::runtime_error;

FILE *debugfp = NULL;
#ifdef DEBUG
std::ofstream logStream;
#else
onullstream logStream;
#endif

#define DEFUN(a) a

#pragma comment(lib,"crypt32.lib")

typedef struct _BCRYPT_PKCS1_PADDING_INFO_adhoc {
  LPCWSTR pszAlgId;
} BCRYPT_PKCS1_PADDING_INFO_adhoc;

#define KEYLEN 1024
#define _ENC_ (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

#define NULLSTR(a) (a == NULL ? "<NULL>" : a)
#define NULLWSTR(a) (a == NULL ? L"<NULL>" : a)

#define AUTH_PIN_ID 1
//#define PUKK_PIN_ID 2
#define SIGN_PIN_ID 3

#define CARDID_LEN 11

#define AUTH_CONTAINER_INDEX 0
#define SIGN_CONTAINER_INDEX 1

typedef struct _CONTAINERMAPRECORD {
    BYTE GuidInfo[80];	// 40 x UNICODE char
    BYTE Flags;		// Bit 1 set for default container
    BYTE RFUPadding;	    
    WORD ui16SigKeySize;
    WORD ui16KeyExchangeKeySize;
} CONTAINERMAPREC;

struct cardFiles {
	BYTE file_appdir[9];
	BYTE file_cardcf[6];
	BYTE file_cardid[16];
};

LPBYTE file_cmap[sizeof(CONTAINERMAPREC)];

DWORD ret(ErrCodes a) {
	DWORD ret = a;
#ifdef DEBUG
	if (a == 0x8010001F || a == 0x0000051A || a == SCARD_E_FILE_NOT_FOUND ||
		a == 0x8010006a) {
		int k = 1;
		}
	if (a == 0x80090009 ) {
		int f = 0;
		}
	if (a != E_OK /*&& a!= 0x80100022 */) {
		int fck = 0;
		}
#endif
	if (a == E_OK) {
		dbgPrintf("return OK\n"); }
	else {
		dbgPrintf("return error 0x%08X\n",ret);
		}
	return a;
	}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
#ifndef DEBUG
//			logStream.setstate(std::ios_base::eofbit);
#else
		{
				 _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
					 _CRTDBG_CHECK_ALWAYS_DF |_CRTDBG_CHECK_CRT_DF
					 |_CRTDBG_DELAY_FREE_MEM_DF |_CRTDBG_LEAK_CHECK_DF
					 );
				WCHAR _cname[MAX_PATH * 4 ] = L"\0";
				PWCHAR cname = _cname + 1;
				HMODULE caller = GetModuleHandle(NULL);
				GetModuleFileName(caller,cname,MAX_PATH);
				PWCHAR fl = (PTCHAR )cname  + lstrlen(cname) -1;
				while (isalnum(*fl) || (L'.' == *fl) || (L'_' == *fl)) fl--;
				fl++; 
				if (*fl == L'\0') return FALSE;
				if (!lstrcmpi(fl,L"explorer.exe")) return FALSE;
				if (!lstrcmpi(fl,L"lsass.exe")) return FALSE;
				if (!lstrcmpi(fl,L"winlogon.exe"))return FALSE;
				if (!lstrcmpi(fl,L"svchost.exe"))return FALSE;
				WCHAR logfile[MAX_PATH * 4];
				GetTempPath(sizeof(logfile)/sizeof(WCHAR),logfile);
				_snwprintf(logfile + wcslen(logfile),
					MAX_PATH,L"esteidcm%u.log",GetCurrentProcessId());
				debugfp = _wfopen(logfile,L"a+");
				if (!debugfp) return FALSE;
				wstring path = wstring(logfile) + L".APDU.log";
				logStream.open(path.c_str());
				dbgPrintf("DllMain:DLL_PROCESS_ATTACH %S",NULLWSTR(fl));
				}
#endif

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
#ifdef DEBUG
		if (debugfp) {dbgPrintf("DllMain:DLL_PROCESS_DETACH");fclose(debugfp);}
#endif
		break;
	}
    return TRUE;
}

/*
HRESULT DllRegisterServer(
						  BOOL bRegTypeLib) {
	return TRUE;
}

 
HRESULT DllUnregisterServer(BOOL bUnRegTypeLib) {
	return TRUE;
	}
*/
/*
PFN_CSP_ALLOC pfnMyCspAlloc = NULL;
PFN_CSP_REALLOC pfnMyCspReAlloc = NULL;
PFN_CSP_FREE  pfnMyCspFree = NULL;
*/

DWORD WINAPI
CardAcquireContext(
    IN      PCARD_DATA  pCardData,
	__in    DWORD       dwFlags) {

	if (!pCardData) 
		return ret(E_PARAM);
	if (dwFlags) return ret(E_PARAM);

	dbgPrintf("CardAcquireContext, dwVersion=%u, name=%S"
		", hScard=0x%08X, hSCardCtx=0x%08X"
		,pCardData->dwVersion,NULLWSTR(pCardData->pwszCardName)
		,pCardData->hScard, pCardData->hSCardCtx
		);

	DWORD suppliedVersion = pCardData->dwVersion;
	if (pCardData->dwVersion < 6 && pCardData->dwVersion != 0 //COMPAT
		//|| pCardData->dwVersion > 6
		) 
		return ret(E_REVISION);

	if (suppliedVersion == 0 && pCardData->cbAtr != 0) //special case
		return ret(E_REVISION);

	pCardData->dwVersion = 6;

	pCardData->pfnCardDeleteContext = CardDeleteContext;
    pCardData->pfnCardQueryCapabilities = CardQueryCapabilities;
    pCardData->pfnCardDeleteContainer= CardDeleteContainer;
    pCardData->pfnCardCreateContainer= CardCreateContainer;
    pCardData->pfnCardGetContainerInfo= CardGetContainerInfo;
    pCardData->pfnCardAuthenticatePin= CardAuthenticatePin;
    pCardData->pfnCardGetChallenge= CardGetChallenge;
    pCardData->pfnCardAuthenticateChallenge= CardAuthenticateChallenge;
    pCardData->pfnCardUnblockPin= CardUnblockPin;
    pCardData->pfnCardChangeAuthenticator= CardChangeAuthenticator;
    pCardData->pfnCardDeauthenticate= NULL;// CardDeauthenticate; //CardDeauthenticate; 
    pCardData->pfnCardCreateDirectory= CardCreateDirectory;
    pCardData->pfnCardDeleteDirectory= CardDeleteDirectory;
    pCardData->pvUnused3= NULL;
    pCardData->pvUnused4= NULL;
    pCardData->pfnCardCreateFile= CardCreateFile;
    pCardData->pfnCardReadFile= CardReadFile;
    pCardData->pfnCardWriteFile= CardWriteFile;
    pCardData->pfnCardDeleteFile= CardDeleteFile;
    pCardData->pfnCardEnumFiles= CardEnumFiles;
    pCardData->pfnCardGetFileInfo= CardGetFileInfo;
    pCardData->pfnCardQueryFreeSpace= CardQueryFreeSpace;
    pCardData->pfnCardQueryKeySizes= CardQueryKeySizes;

    pCardData->pfnCardSignData= CardSignData;
    pCardData->pfnCardRSADecrypt= CardRSADecrypt;
    pCardData->pfnCardConstructDHAgreement= NULL;//CardConstructDHAgreement;

	if (suppliedVersion !=0 ) {
		if (NULL == pCardData->pbAtr ) return ret(E_PARAM);
		switch(pCardData->cbAtr) {
			case 26:
			case 25:
			case 18: {
				const char ref[] = "EstEID ver 1.0";
				unsigned char *ptr = pCardData->pbAtr + pCardData->cbAtr - sizeof(ref) + 2;
				while(--ptr > pCardData->pbAtr)
					if (!memcmp(ref,ptr,sizeof(ref)-1)) break;
				if (ptr == pCardData->pbAtr) 
					return ret(E_WRONG_CARD);
				break;
				}
			default:
				return ret(E_PARAM);
			}

		if (NULL == pCardData->pwszCardName ) return ret(E_PARAM);

		if (NULL == pCardData->pfnCspAlloc) return ret(E_PARAM);
		if (NULL == pCardData->pfnCspReAlloc) return ret(E_PARAM);
		if (NULL == pCardData->pfnCspFree) return ret(E_PARAM);

		pCardData->pvVendorSpecific = pCardData->pfnCspAlloc(sizeof(cardFiles));
		if (!pCardData->pvVendorSpecific) return ret(E_MEMORY);
		BYTE empty_appdir[] = {1,'m','s','c','p',0,0,0,0};
		BYTE empty_cardcf[6]={0,0,0,0,0,0};
		BYTE empty_cardid[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
		memcpy(((cardFiles *)pCardData->pvVendorSpecific)->file_appdir,empty_appdir,sizeof(empty_appdir));
		memcpy(((cardFiles *)pCardData->pvVendorSpecific)->file_cardcf,empty_cardcf,sizeof(empty_cardcf));
		memcpy(((cardFiles *)pCardData->pvVendorSpecific)->file_cardid,empty_cardid,sizeof(empty_cardid));
/*
		if (NULL == pCardData->pfnCspCacheAddFile) return ret(E_PARAM);
		if (NULL == pCardData->pfnCspCacheLookupFile) return ret(E_PARAM);
		if (NULL == pCardData->pfnCspCacheDeleteFile) return ret(E_PARAM);
*/
		if (0 == pCardData->hScard ) return ret(E_BADHANDLE);
		//if (0 == pCardData->hSCardCtx ) return ret(E_BADHANDLE);
	}
	if (suppliedVersion > 4) {
		pCardData->pfnCardDeriveKey = NULL; //CardDeriveKey;
		pCardData->pfnCardDestroyDHAgreement = NULL; //CardDestroyDHAgreement;
		pCardData->pfnCspGetDHAgreement = NULL; //CspGetDHAgreement ;

		if (suppliedVersion > 5 ) {
			pCardData->pfnCardGetChallengeEx = CardGetChallengeEx;
			pCardData->pfnCardAuthenticateEx = CardAuthenticateEx;
			pCardData->pfnCardChangeAuthenticatorEx = CardChangeAuthenticatorEx;
			pCardData->pfnCardDeauthenticateEx = CardDeauthenticateEx; //CardDeauthenticateEx ;
			pCardData->pfnCardGetContainerProperty = CardGetContainerProperty ;
			pCardData->pfnCardSetContainerProperty = CardSetContainerProperty;
			pCardData->pfnCardGetProperty = CardGetProperty;
			pCardData->pfnCardSetProperty = CardSetProperty;
			}
		}
	return ret(E_OK);
	}


DWORD WINAPI
CardDeleteContext(
				  __inout     PCARD_DATA  pCardData) {
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardDeleteContext");
	if (pCardData->pvVendorSpecific)
		pCardData->pfnCspFree(pCardData->pvVendorSpecific);
	return ret(E_OK);}


DWORD WINAPI 
CardGetContainerProperty(
    __in   PCARD_DATA                                 pCardData,
    __in   BYTE                                       bContainerIndex,
    __in   LPCWSTR                                    wszProperty,
    __out_bcount_part_opt(cbData, *pdwDataLen) PBYTE  pbData,
    __in   DWORD                                      cbData,
    __out  PDWORD                                     pdwDataLen,
    __in   DWORD                                      dwFlags
	) {
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardGetContainerProperty bContainerIndex=%u, wszProperty=%S"
		", cbData=%u, dwFlags=0x%08X"
		,bContainerIndex,NULLWSTR(wszProperty),cbData,dwFlags);
	if (!wszProperty) 
		return ret(E_PARAM);
	if (dwFlags) 
		return ret(E_PARAM);
	if (!pbData)
		return ret(E_PARAM);
	if (!pdwDataLen) 
		return ret(E_PARAM);

	if (wstring(CCP_CONTAINER_INFO) == wszProperty ) {
		PCONTAINER_INFO p = (PCONTAINER_INFO) pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData >= sizeof(DWORD))
			if (p->dwVersion != CONTAINER_INFO_CURRENT_VERSION && 
				p->dwVersion != 0 ) return ret(E_REVISION);
		if (cbData < sizeof(*p)) return ret(E_BUFFER);
		dbgPrintf("-->calling CardGetContainerInfo");
		DWORD code = CardGetContainerInfo(pCardData,bContainerIndex,0,p);
		dbgPrintf("<--calling CardGetContainerInfo");
		return code;
		}
	if (wstring(CCP_PIN_IDENTIFIER) == wszProperty ) {
		PPIN_ID p = (PPIN_ID) pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_BUFFER);
		switch (bContainerIndex) {
			case AUTH_CONTAINER_INDEX:
				*p = AUTH_PIN_ID;
				break;
			case SIGN_CONTAINER_INDEX:
				*p = SIGN_PIN_ID;
				break;
			default:
				return ret(E_NOCONTAINER);
			}
		dbgPrintf("Return Pin id %u",*p);
		return ret(E_OK);
		}

	return ret(E_PARAM);
}

DWORD WINAPI 
CardSetContainerProperty(
    __in   PCARD_DATA                                 pCardData,
    __in   BYTE                                       bContainerIndex,
    __in   LPCWSTR                                    wszProperty,
    __in_bcount(cbDataLen)  PBYTE                     pbData,
    __in   DWORD                                      cbDataLen,
    __in   DWORD                                      dwFlags
	) {
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardSetContainerProperty bContainerIndex=%u, wszProperty=%S"
		", cbDataLen=%u, dwFlags=0x%08X"
		,bContainerIndex,NULLWSTR(wszProperty),cbDataLen,dwFlags);
	return ret(E_UNSUPPORTED);
}
DWORD WINAPI 
CardGetProperty(
    __in   PCARD_DATA                                 pCardData,
    __in   LPCWSTR                                    wszProperty,
    __out_bcount_part_opt(cbData, *pdwDataLen) PBYTE  pbData,
    __in   DWORD                                      cbData,
    __out  PDWORD                                     pdwDataLen,
    __in   DWORD                                      dwFlags
	) {
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardGetProperty wszProperty=%S, cbData=%u, dwFlags=%u"
		,NULLWSTR(wszProperty),cbData,dwFlags);
	if (!wszProperty) return ret(E_PARAM);
	if (!pbData) return ret(E_PARAM);
	if (!pdwDataLen) return ret(E_PARAM);

	if (wstring(CP_CARD_PIN_STRENGTH_CHANGE) == wszProperty ||
		wstring(CP_CARD_PIN_STRENGTH_UNBLOCK) == wszProperty)
		return ret(E_UNSUPPORTED);

	if (dwFlags) {
		if (wstring(CP_CARD_PIN_INFO) != wszProperty && 
			wstring(CP_CARD_PIN_STRENGTH_VERIFY) != wszProperty &&
			wstring(CP_CARD_KEYSIZES) != wszProperty) 
			return ret(E_PARAM);
		if (dwFlags > SIGN_PIN_ID ) 
			return ret(E_PARAM);
		}

	if (wstring(CP_CARD_FREE_SPACE) == wszProperty ) {
		PCARD_FREE_SPACE_INFO p = (PCARD_FREE_SPACE_INFO )pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		if (p->dwVersion > CARD_FREE_SPACE_INFO_CURRENT_VERSION ) return ret(E_REVISION);
		p->dwVersion = CARD_FREE_SPACE_INFO_CURRENT_VERSION;
		p->dwBytesAvailable = 0;
		p->dwKeyContainersAvailable = 0;
		p->dwMaxKeyContainers = 2;
		return ret(E_OK);
		}
	if (wstring(CP_CARD_CAPABILITIES) == wszProperty ) {
		PCARD_CAPABILITIES p = (PCARD_CAPABILITIES )pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		if (p->dwVersion != CARD_CAPABILITIES_CURRENT_VERSION &&
			p->dwVersion != 0) return ret(E_REVISION);
		p->fCertificateCompression = TRUE;
		p->fKeyGen = FALSE;
		return ret(E_OK);
		}
	if (wstring(CP_CARD_KEYSIZES) == wszProperty) {
		PCARD_KEY_SIZES p = (PCARD_KEY_SIZES )pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		if (p->dwVersion != CARD_KEY_SIZES_CURRENT_VERSION &&
			p->dwVersion != 0) return ret(E_REVISION);
		p->dwDefaultBitlen = 1024;
		p->dwIncrementalBitlen = 0;
		p->dwMaximumBitlen = 1024;
		p->dwMinimumBitlen = 1024;
		return ret(E_OK);
		}
	if (wstring(CP_CARD_READ_ONLY) == wszProperty) {
		BOOL *p = (BOOL*)pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		*p = TRUE; //HACK
		return ret(E_OK);
		}
	if (wstring(CP_CARD_CACHE_MODE) == wszProperty) {
		DWORD *p = (DWORD *)pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		*p = CP_CACHE_MODE_SESSION_ONLY;
		return ret(E_OK);
		}
	if (wstring(L"Supports Windows x.509 Enrollment") == wszProperty) {
		DWORD *p = (DWORD *)pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		*p = 0;
		return ret(E_OK);
		}
	if (wstring(CP_CARD_GUID) == wszProperty) {
		cardFiles *ptr = (cardFiles *)pCardData->pvVendorSpecific;
		if (pdwDataLen) *pdwDataLen = sizeof(ptr->file_cardid);
		if (cbData < sizeof(ptr->file_cardid)) return ret(E_SCBUFFER);
		try {
			PCSCManager mgr(pCardData->hSCardCtx);
			EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
			scard.setLogging(&logStream);
			string id  = scard.readCardID();
			if (id.length() != CARDID_LEN) {
				dbgPrintf("runtime_error in CardReadFile id.length() is %d",id.length());
				return ret(E_NOFILE);
				}
			memset(ptr->file_cardid,0, sizeof(ptr->file_cardid));
			CopyMemory( ptr->file_cardid, id.c_str(), id.length());
			dbgPrintf("cardid: '%s'",ptr->file_cardid);
		} catch (runtime_error &err ) {
			dbgPrintf("runtime_error in CardReadFile '%s'",err.what());
			return ret(E_NOFILE);
			}

		CopyMemory(pbData,ptr->file_cardid,sizeof(ptr->file_cardid));
		return ret(E_OK);
		}
	if (wstring(CP_CARD_SERIAL_NO) == wszProperty) {
		return ret(E_PARAM);
		}
	if (wstring(CP_CARD_PIN_INFO) == wszProperty) {
		PPIN_INFO p = (PPIN_INFO) pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		if (p->dwVersion != 6) return ret(E_REVISION);
		p->PinType = AlphaNumericPinType;
		p->dwFlags = 0;
		switch (dwFlags) {
			case AUTH_PIN_ID:
				dbgPrintf("returning info on PIN 1 ( Auth ) [%u]",dwFlags);
				p->PinPurpose = AuthenticationPin;
				p->PinCachePolicy.dwVersion = 6;
				p->PinCachePolicy.dwPinCachePolicyInfo = 0;
				p->PinCachePolicy.PinCachePolicyType = PinCacheNormal;
				p->dwChangePermission = 1 << (AUTH_PIN_ID) ;
				p->dwUnblockPermission = 0; //1 << (PUKK_PIN_ID);
				break;
			case SIGN_PIN_ID:
				dbgPrintf("returning info on PIN 2 ( Sign ) [%u]",dwFlags);
				p->PinPurpose = DigitalSignaturePin;
				p->PinCachePolicy.dwVersion = 6;
				p->PinCachePolicy.dwPinCachePolicyInfo = 0;
				p->PinCachePolicy.PinCachePolicyType = PinCacheNone;
				p->dwChangePermission = 1 << (SIGN_PIN_ID) ;
				p->dwUnblockPermission = 0; //1 << (PUKK_PIN_ID);
				break;
/*			case PUKK_PIN_ID:
				dbgPrintf("returning info on PUK [%u]",dwFlags);
				p->PinPurpose = AdministratorPin;
				p->PinCachePolicy.dwVersion = 6;
				p->PinCachePolicy.dwPinCachePolicyInfo = 0;
				p->PinCachePolicy.PinCachePolicyType = PinCacheNone;
				p->dwChangePermission = 1 << (PUKK_PIN_ID) ;
				p->dwUnblockPermission = 0;
				p->PinType = EmptyPinType;
#ifdef WORKAROUND
				p->PinType = ChallengeResponsePinType;
#endif
				break;
*/
/*			case ROLE_ADMIN:
			case ROLE_USER:
				dbgPrintf("User pin info %u requested",dwFlags);
				p->PinType = EmptyPinType;
				p->PinPurpose = AuthenticationPin;
				p->PinCachePolicy.dwVersion = 6;
				p->PinCachePolicy.dwPinCachePolicyInfo = 0;
				p->PinCachePolicy.PinCachePolicyType = PinCacheNone;
				p->dwChangePermission = 0 ;
				p->dwUnblockPermission = 0;
				break;*/
			default:
				dbgPrintf("Invalid Pin number %u requested",dwFlags);
				return ret(E_PARAM);
			}
		return ret(E_OK);
		}
	if (wstring(CP_CARD_LIST_PINS) == wszProperty) {
		PPIN_SET p = (PPIN_SET) pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		SET_PIN(*p,AUTH_PIN_ID);
		//SET_PIN(*p,2);
		SET_PIN(*p,SIGN_PIN_ID);
		return ret(E_OK);
		}
	if (wstring(CP_CARD_AUTHENTICATED_STATE) == wszProperty) {
		PPIN_SET p = (PPIN_SET) pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		return ret(E_PARAM);
		}
	if (wstring(CP_CARD_PIN_STRENGTH_VERIFY) == wszProperty) {
		if (dwFlags < AUTH_PIN_ID || dwFlags > SIGN_PIN_ID) return ret(E_PARAM);
		DWORD *p = (DWORD *)pbData;
		if (pdwDataLen) *pdwDataLen = sizeof(*p);
		if (cbData < sizeof(*p)) return ret(E_SCBUFFER);
		*p = CARD_PIN_STRENGTH_PLAINTEXT;
		return ret(E_OK);
		}
	if (wstring(CP_CARD_PIN_STRENGTH_CHANGE) == wszProperty) {
		return ret(E_UNSUPPORTED);
		}
	if (wstring(CP_CARD_PIN_STRENGTH_UNBLOCK) == wszProperty) {
		return ret(E_UNSUPPORTED);
		}
	dbgPrintf("fell through..");
	return ret(E_PARAM);
}

DWORD WINAPI 
CardSetProperty(
    __in   PCARD_DATA                                 pCardData,
    __in   LPCWSTR                                    wszProperty,
    __in_bcount(cbDataLen)  PBYTE                     pbData,
    __in   DWORD                                      cbDataLen,
    __in   DWORD                                      dwFlags
	) {
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardSetProperty wszProperty=%S"
		", cbDataLen=%u, dwFlags=%u"
		,NULLWSTR(wszProperty),cbDataLen,dwFlags);
	if (!wszProperty) return ret(E_PARAM);

	if (wstring(CP_CARD_PIN_STRENGTH_VERIFY) == wszProperty ||
		wstring(CP_CARD_PIN_INFO) == wszProperty)  return ret(E_NEEDSAUTH);

	if (dwFlags) return ret(E_PARAM);

	if (wstring(CP_PIN_CONTEXT_STRING) == wszProperty)
		return ret(E_OK);

	if (wstring(CP_CARD_CACHE_MODE) == wszProperty || 
		wstring(CP_SUPPORTS_WIN_X509_ENROLLMENT) == wszProperty ||
		wstring(CP_CARD_GUID) == wszProperty ||
		wstring(CP_CARD_SERIAL_NO)  == wszProperty ) {
		return ret(E_NEEDSAUTH);
		}

	if (!pbData) return ret(E_PARAM);
	if (!cbDataLen) return ret(E_PARAM);

	if (wstring(CP_PARENT_WINDOW) == wszProperty) {
		if (cbDataLen != sizeof(DWORD)) 
			return ret(E_PARAM);
		HWND cp = *((HWND *) pbData);
		if (cp!=0 && !IsWindow(cp)) 
			return ret(E_PARAM);
		return ret(E_OK);
		}


	return ret(E_PARAM);
}


DWORD WINAPI
CardQueryCapabilities(
    __in      PCARD_DATA          pCardData,
    __in      PCARD_CAPABILITIES  pCardCapabilities){
	if (!pCardData) return ret(E_PARAM);
	if (!pCardCapabilities) return ret(E_PARAM);

	if (pCardCapabilities->dwVersion != CARD_CAPABILITIES_CURRENT_VERSION
		&& pCardCapabilities->dwVersion != 0
		)
		return ret(E_REVISION);

	pCardCapabilities->dwVersion = CARD_CAPABILITIES_CURRENT_VERSION;
	dbgPrintf("CardQueryCapabilities dwVersion=%u, fKeyGen=%u, fCertificateCompression=%u"
 		,pCardCapabilities->dwVersion,pCardCapabilities->fKeyGen 
		,pCardCapabilities->fCertificateCompression   );
	
	pCardCapabilities->fCertificateCompression = TRUE;
	pCardCapabilities->fKeyGen = FALSE;
	return ret(E_OK);
	}

typedef struct {
	PUBLICKEYSTRUC  publickeystruc;
	RSAPUBKEY rsapubkey;
	BYTE modulus[KEYLEN/8];
	BYTE prime1[KEYLEN/16];
	BYTE prime2[KEYLEN/16];
	BYTE exponent1[KEYLEN/16];
	BYTE exponent2[KEYLEN/16];
	BYTE coefficient[KEYLEN/16];
	BYTE privateExponent[KEYLEN/8];
} RSAPRIV;

DWORD WINAPI
CardCreateContainer(
    __in      PCARD_DATA  pCardData,
    __in      BYTE        bContainerIndex,
    __in      DWORD       dwFlags,
    __in      DWORD       dwKeySpec,
    __in      DWORD       dwKeySize,
    __in      PBYTE       pbKeyData) {
	return ret(E_UNSUPPORTED);
	}


typedef struct {
	PUBLICKEYSTRUC  publickeystruc;
	RSAPUBKEY rsapubkey;
	BYTE modulus[KEYLEN/8]; 
} PUBKEYSTRUCT;

DWORD WINAPI
CardGetContainerInfo(
    __in      PCARD_DATA  pCardData,
    __in      BYTE        bContainerIndex,
    __in      DWORD       dwFlags,
    __in      PCONTAINER_INFO pContainerInfo){
	if (!pCardData) return ret(E_PARAM);
	if (!pContainerInfo) return ret(E_PARAM);
	if (dwFlags) return ret(E_PARAM);
	if (pContainerInfo->dwVersion < 0 
		|| pContainerInfo->dwVersion >  CONTAINER_INFO_CURRENT_VERSION) 
		return ret(E_REVISION);

	dbgPrintf("CardGetContainerInfo bContainerIndex=%u, dwFlags=0x%08X, dwVersion=%u"
		", cbSigPublicKey=%u, cbKeyExPublicKey=%u"
		,bContainerIndex,dwFlags , pContainerInfo->dwVersion 
		,pContainerInfo->cbSigPublicKey, pContainerInfo->cbKeyExPublicKey 
		);

	if (bContainerIndex != SIGN_CONTAINER_INDEX && 
		bContainerIndex != AUTH_CONTAINER_INDEX) 
		return ret(E_NOCONTAINER);

	if (bContainerIndex != AUTH_CONTAINER_INDEX && pCardData->dwVersion < 6 ) {
		dbgPrintf("Version %u requested container %u",pCardData->dwVersion,bContainerIndex);
		return ret(E_NOCONTAINER);
		}

	PUBKEYSTRUCT oh;
	DWORD sz = sizeof(oh);

	try {
		ByteVec reply;
		PCSCManager mgr(pCardData->hSCardCtx);
		EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
		scard.setLogging(&logStream);

		if (bContainerIndex == AUTH_CONTAINER_INDEX)
			reply = scard.getAuthCert();
		else 
			reply = scard.getSignCert();

		PCCERT_CONTEXT cer = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING 
			,&reply[0],(DWORD) reply.size());
		PCERT_PUBLIC_KEY_INFO pinf = &(cer->pCertInfo->SubjectPublicKeyInfo);
		CryptDecodeObject(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING ,
			RSA_CSP_PUBLICKEYBLOB, pinf->PublicKey.pbData , pinf->PublicKey.cbData ,
			0, (LPVOID) &oh, &sz);
	} catch (runtime_error & ex) {
		dbgPrintf("runtime_error exception thrown:", ex.what());
		return ret(E_INTERNAL);
		}

	if (bContainerIndex == AUTH_CONTAINER_INDEX) {
		oh.publickeystruc.aiKeyAlg = CALG_RSA_KEYX;
		pContainerInfo->cbKeyExPublicKey = sz;
		pContainerInfo->pbKeyExPublicKey = (PBYTE)(*pCardData->pfnCspAlloc)(sz);
		if (!pContainerInfo->pbKeyExPublicKey) return ret(E_MEMORY);
		pContainerInfo->cbSigPublicKey = 0;//sz;
		pContainerInfo->pbSigPublicKey = NULL;//(PBYTE)(*pCardData->pfnCspAlloc)(sz);
		CopyMemory(pContainerInfo->pbKeyExPublicKey,&oh,sz);
		//CopyMemory(pContainerInfo->pbSigPublicKey,&oh,sz);
		dbgPrintf("return info on AUTH_CONTAINER_INDEX");
		}
	else { //SIGN_CONTAINER_INDEX
		oh.publickeystruc.aiKeyAlg = CALG_RSA_SIGN;
		pContainerInfo->cbKeyExPublicKey = 0;
		pContainerInfo->pbKeyExPublicKey = NULL;
		pContainerInfo->cbSigPublicKey = sz;
		pContainerInfo->pbSigPublicKey = (PBYTE)(*pCardData->pfnCspAlloc)(sz);
		if (!pContainerInfo->pbSigPublicKey) return ret(E_MEMORY);
		CopyMemory(pContainerInfo->pbSigPublicKey,&oh,sz);
		dbgPrintf("return info on SIGN_CONTAINER_INDEX");
		}

	pContainerInfo->dwVersion = CONTAINER_INFO_CURRENT_VERSION;
	return ret(E_OK);
	}


DWORD WINAPI
CardAuthenticatePin(
    __in                   PCARD_DATA  pCardData,
    __in                   LPWSTR      pwszUserId,
    __in_bcount(cbPin)     PBYTE       pbPin,
    __in                   DWORD       cbPin,
    __out_opt              PDWORD pcAttemptsRemaining){
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardAuthenticatePin pwszUserId=%S, cbPin=%u"
		,NULLWSTR(pwszUserId),cbPin);

	if (NULL == pwszUserId) return ret(E_PARAM);
	if (wstring(wszCARD_USER_USER) != pwszUserId &&
		wstring(wszCARD_USER_ADMIN) != pwszUserId
		) return ret(E_PARAM);

	if (NULL == pbPin) return ret(E_PARAM);

	//wrongpin info should be returned before ..
	//if (pcAttemptsRemaining) *pcAttemptsRemaining = 3;

/*	if (!wcscmp(wszCARD_USER_USER,pwszUserId)) {
		const unsigned char ref[] = "1234";
		if (cbPin != 4) return ret(E_WRONGPIN);
		if (!memcmp(pbPin,ref,4))
			return ret(E_OK);
		return ret(E_WRONGPIN);
		}*/

	if (cbPin < 4 || cbPin > 12) return ret(E_WRONGPIN);

	if (wstring(wszCARD_USER_ADMIN) == pwszUserId) {
		return ret(E_WRONGPIN);
		}

	char *pin = (char *)pbPin;
	PinString tmp(pin , pin+cbPin );
	BYTE remaining = 0,dummy = 0xFA;
	//TODO: validate the presented pin here
	try {
		PCSCManager mgr(pCardData->hSCardCtx);
		EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
		scard.setLogging(&logStream);
		if (pcAttemptsRemaining) {
			byte puk,pinSign;
			scard.getRetryCounts(puk,remaining,pinSign);
			}
		try {
			scard.validateAuthPin(tmp,dummy);
		} catch (SCError &err) {
			if (err.error == SCARD_E_INVALID_VALUE) { //certseq specific hack
				scard.endTransaction();
				scard.validateAuthPin(tmp,dummy);
			} else throw err;
			}
	} catch (runtime_error &ex) {
		if (pcAttemptsRemaining) *pcAttemptsRemaining = remaining - 1;
		dbgPrintf("exception %s",ex.what());
		return ret(E_WRONGPIN);
		}

	return ret(E_OK);
	}

DWORD WINAPI 
CardAuthenticateEx(
    __in   PCARD_DATA                             pCardData,
    __in   PIN_ID                                 PinId,
    __in   DWORD                                  dwFlags,
    __in   PBYTE                                  pbPinData,
    __in   DWORD                                  cbPinData,
    __deref_out_bcount_opt(*pcbSessionPin) PBYTE  *ppbSessionPin,
    __out_opt PDWORD                              pcbSessionPin,
	__out_opt PDWORD                              pcAttemptsRemaining
	) {
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardAuthenticateEx: PinId=%u, dwFlags=0x%08X, cbPinData=%u, Attempts %s"
		,PinId,dwFlags,cbPinData,pcAttemptsRemaining ? "YES" : "NO");


	if (dwFlags == CARD_AUTHENTICATE_GENERATE_SESSION_PIN ||
		dwFlags == CARD_AUTHENTICATE_SESSION_PIN)
		return ret(E_UNSUPPORTED);
	if (dwFlags && dwFlags != CARD_PIN_SILENT_CONTEXT) 
		return ret(E_PARAM);

	if (NULL == pbPinData) return ret(E_PARAM);

	if (pcAttemptsRemaining) {
		*pcAttemptsRemaining = 3;
		}

	if (cbPinData < 4 || cbPinData > 12) return ret(E_WRONGPIN);

	char *pin = (char *)pbPinData;

	PinString tmp(pin , pin+cbPinData );
	BYTE remaining = 0,dummy = 0xFA;
	byte puk,pinAuth = 0,pinSign = 0;
	if (PinId != AUTH_PIN_ID && PinId != SIGN_PIN_ID /*&& PinId != PUKK_PIN_ID*/) 
		return ret(E_PARAM);
	try {
		PCSCManager mgr(pCardData->hSCardCtx);
		EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
		scard.setLogging(&logStream);
		if (pcAttemptsRemaining) {
			scard.getRetryCounts(puk,pinAuth,pinSign);
			}
		if (PinId == AUTH_PIN_ID) {
			dbgPrintf("CardAuthenticateEx: pbPinData=%s, auth Pin",tmp.c_str());
			remaining = pinAuth;
			scard.validateAuthPin(tmp,dummy);
			}
		if (PinId == SIGN_PIN_ID) {
			dbgPrintf("CardAuthenticateEx: pbPinData=%s, sign Pin",tmp.c_str());
			remaining = pinSign;
			scard.validateSignPin(tmp,dummy);
			}
	} catch (AuthError e) {
		if (pcAttemptsRemaining) *pcAttemptsRemaining = remaining - 1;
		return ret(e.m_blocked ? E_PINBLOCKED : E_WRONGPIN );
	} catch (runtime_error & ) {
		if (pcAttemptsRemaining) *pcAttemptsRemaining = remaining - 1;
		return ret(E_WRONGPIN);
		}

	if (pcAttemptsRemaining) *pcAttemptsRemaining = remaining - 1;

	return ret(E_OK);
}


DWORD WINAPI
CardEnumFiles(
    __in      PCARD_DATA  pCardData,
    __in      LPSTR       pszDirectoryName,
    __out_ecount(*pdwcbFileName)
              LPSTR      *pmszFileNames,
    __out     LPDWORD     pdwcbFileName,
    __in      DWORD       dwFlags
	){
	const char root_files[] = "cardapps\0cardcf\0cardid\0\0";
	const char mscp_files[] = "kxc00\0kxc01\0cmapfile\0\0";
	if (!pCardData) return ret(E_PARAM);
	if (!pmszFileNames) return ret(E_PARAM);
	if (!pdwcbFileName) return ret(E_PARAM);
	if (dwFlags) return ret(E_PARAM);

	if (!pszDirectoryName || !strlen(pszDirectoryName)) {
		DWORD sz = sizeof(root_files) - 1;
		//if (*pdwcbFileName && *pdwcbFileName < sz) return ret(E_BUFFER);
		LPSTR t = (LPSTR)(*pCardData->pfnCspAlloc)(sz);
		if (!t) return ret(E_MEMORY);
		CopyMemory(t,root_files,sz);
		*pmszFileNames = t;
		*pdwcbFileName = sz;
		return ret(E_OK);
		}
	if (!_strcmpi(pszDirectoryName,"mscp")) {
		DWORD sz = sizeof(mscp_files) - 1;
		//if (*pdwcbFileName && *pdwcbFileName < sz) return ret(E_BUFFER);
		LPSTR t = (LPSTR)(*pCardData->pfnCspAlloc)(sz);
		if (!t) return ret(E_MEMORY);
		CopyMemory(t,mscp_files,sz);
		*pmszFileNames = t;
		*pdwcbFileName = sz;
		return ret(E_OK);
		}
	return ret(E_NODIRECTORY);
	}


DWORD WINAPI
CardGetFileInfo(
    __in         PCARD_DATA  pCardData,
    __in         LPSTR      pszDirectoryName,
    __in         LPSTR      pszFileName,
    __in         PCARD_FILE_INFO pCardFileInfo
	){
	if (!pCardData) return ret(E_PARAM);
	if (!pszFileName) return ret(E_PARAM);
	if (!strlen(pszFileName)) return ret(E_PARAM);
	if (!pCardFileInfo) return ret(E_PARAM);

	if (pCardFileInfo->dwVersion != CARD_FILE_INFO_CURRENT_VERSION && 
		pCardFileInfo->dwVersion != 0 ) 
		return ret(E_REVISION);

	pCardFileInfo->AccessCondition = EveryoneReadUserWriteAc;
	if (!pszDirectoryName || !strlen(pszDirectoryName)) {
		if (!_strcmpi(pszFileName,"cardapps")) {
			pCardFileInfo->cbFileSize = sizeof( ((cardFiles *)pCardData->pvVendorSpecific)->file_appdir);
			return ret(E_OK);
			}
		if (!_strcmpi(pszFileName,"cardcf")) {
			pCardFileInfo->cbFileSize = sizeof(((cardFiles *)pCardData->pvVendorSpecific)->file_cardcf);
			return ret(E_OK);
			}
		if (!_strcmpi(pszFileName,"cardid")) {
			pCardFileInfo->cbFileSize = sizeof(((cardFiles *)pCardData->pvVendorSpecific)->file_cardid);
			return ret(E_OK);
			}
		dbgPrintf("CardGetFileInfo:file not found 0");
		return ret(E_NOFILE);
		}
	if (!_strcmpi(pszDirectoryName,"mscp")) {
		if (!_strcmpi(pszFileName,"cmapfile")) {
			pCardFileInfo->cbFileSize = sizeof(CONTAINERMAPREC ) * 2;
			return ret(E_OK);
			}
		dbgPrintf("CardGetFileInfo:file not found 1");
		return ret(E_NOFILE);
		}
	return ret(E_NODIRECTORY);
}

DWORD WINAPI
CardReadFile(
    __in                             PCARD_DATA  pCardData,
    __in                             LPSTR       pszDirectoryName,
    __in                             LPSTR       pszFileName,
    __in                             DWORD       dwFlags,
    __deref_out_bcount(*pcbData)     PBYTE      *ppbData,
    __out                            PDWORD      pcbData){
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardReadFile pszDirectoryName=%s, pszFileName=%s, dwFlags=0x%08X"
		,NULLSTR(pszDirectoryName),NULLSTR(pszFileName),dwFlags);
	if (!pszFileName) return ret(E_PARAM);
	if (!strlen(pszFileName)) return ret(E_PARAM);
	if (!ppbData) 
		return ret(E_PARAM);
	if (!pcbData) 
		return ret(E_PARAM);
	if (dwFlags) 
		return ret(E_PARAM);

	if (pszDirectoryName) 
		if (_strcmpi(pszDirectoryName,"mscp")) return ret(E_NODIRECTORY);

	if (!_strcmpi(pszFileName,"cardcf")) {
		DWORD sz = sizeof(((cardFiles *)pCardData->pvVendorSpecific)->file_cardcf);
		//if (*pcbData && *pcbData < sz) return ret(E_BUFFER);
		PBYTE t = (LPBYTE)(*pCardData->pfnCspAlloc)(sz);
		if (!t) return ret(E_MEMORY);
		CopyMemory(t,((cardFiles *)pCardData->pvVendorSpecific)->file_cardcf,sz);
		*ppbData = t;
		*pcbData = sz;
		return ret(E_OK);
		}
	if (!_strcmpi(pszFileName,"cardid")) { //readCardID
		cardFiles *ptr = (cardFiles *)pCardData->pvVendorSpecific; 
		try {
			PCSCManager mgr(pCardData->hSCardCtx);
			EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
			scard.setLogging(&logStream);
			string id  = scard.readCardID();
			if (id.length() != CARDID_LEN) {
				dbgPrintf("runtime_error in CardReadFile id.length() is %d",id.length());
				return ret(E_NOFILE);
				}
			memset(ptr->file_cardid,0, sizeof(ptr->file_cardid));
			CopyMemory( ptr->file_cardid, id.c_str(), id.length());
			dbgPrintf("cardid: '%s'",ptr->file_cardid);
		} catch (runtime_error &err ) {
			dbgPrintf("runtime_error in CardReadFile '%s'",err.what());
			return ret(E_NOFILE);
			}

		DWORD sz = sizeof(ptr->file_cardid);
		//if (*pcbData && *pcbData < sz) return ret(E_BUFFER);
		PBYTE t = (PBYTE)(*pCardData->pfnCspAlloc)(sz);
		if (!t) return ret(E_MEMORY);
		CopyMemory(t,ptr->file_cardid,sz );
		*ppbData = t;
		*pcbData = sz;
		return ret(E_OK);
		}
	if (pszDirectoryName && !_strcmpi(pszDirectoryName,"mscp")) {
		if (!_strcmpi(pszFileName,"kxc00")) {
			ByteVec reply;
			try {
				PCSCManager mgr(pCardData->hSCardCtx);
				EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
				scard.setLogging(&logStream);
				reply = scard.getAuthCert();
			} catch (runtime_error & err) {
				dbgPrintf("runtime_error in CardReadFile, reading kxc00, '%s'",err.what());
				return ret(E_NOFILE);
				}
			
			DWORD sz = (DWORD) reply.size();
			//if (*pcbData && *pcbData < sz) return ret(E_BUFFER);
			PBYTE t = (PBYTE)(*pCardData->pfnCspAlloc)(sz);
			if (!t) return ret(E_MEMORY);
			CopyMemory(t,&reply[0],sz );
			*ppbData = t;
			*pcbData = sz;
			return ret(E_OK);
			}
		if (!_strcmpi(pszFileName,"ksc01")) {
			if (pCardData->dwVersion < 6 ) {
				dbgPrintf("runtime_error in CardReadFile, reading ksc01,pCardData->dwVersion is %d",pCardData->dwVersion);
				return ret(E_NOFILE);
				}

			ByteVec reply;
			try {
				PCSCManager mgr(pCardData->hSCardCtx);
				EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
				scard.setLogging(&logStream);
				reply = scard.getSignCert();
			} catch (runtime_error &err ) {
				dbgPrintf("runtime_error in CardReadFile, reading ksc01, '%s'",err.what());
				return ret(E_NOFILE);
				}
			
			DWORD sz = (DWORD) reply.size();
			//if (*pcbData && *pcbData < sz) return ret(E_BUFFER);
			PBYTE t = (PBYTE)(*pCardData->pfnCspAlloc)(sz);
			if (!t) return ret(E_MEMORY);
			CopyMemory(t,&reply[0],sz );
			*ppbData = t;
			*pcbData = sz;
			return ret(E_OK);
			}

		if (!_strcmpi(pszFileName,"cmapfile")) {
			DWORD numContainers =1;
			if (pCardData->dwVersion >= 6 ) numContainers = 2;

			string id = "";
			size_t i;
			try {
				PCSCManager mgr(pCardData->hSCardCtx);
				EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
				scard.setLogging(&logStream);
				id  = scard.readCardID();
			} catch (runtime_error & err) {
				dbgPrintf("runtime_error in CardReadFile, reading cmapfile '%s'",err.what());
				return ret(E_NOFILE);
				}
			
			if (id.length() != CARDID_LEN) {
				dbgPrintf("runtime_error in CardReadFile, id.length is '%d'",id.length());
				return ret(E_NOFILE);
				}

			WCHAR autGuid[] = L"00000000000_AUT";
			WCHAR sigGuid[] = L"00000000000_SIG";
			for(i = 0;i<id.length();i++) {
				char b = id[i];
				sigGuid[i] = autGuid[i] = b;
			}

			DWORD sz = sizeof(CONTAINERMAPREC ) * numContainers;
			//if ((*pcbData && *pcbData < sz)) {*pcbData = sz;return ret(E_SCBUFFER);}
			PBYTE t = (LPBYTE)(*pCardData->pfnCspAlloc)(sz);
			if (!t) return ret(E_MEMORY);
			PBYTE originalT = t;
			ZeroMemory(t,sz);

			CONTAINERMAPREC *c1 = (CONTAINERMAPREC *) t;
			wcsncpy((PWCHAR ) c1->GuidInfo,autGuid,sizeof(c1->GuidInfo) / 2);
			c1->Flags = 3; // 1 valid + 2 default
			c1->ui16KeyExchangeKeySize = 1024;
			c1->ui16SigKeySize = 0; //1024

			if (numContainers == 2) {
				dbgPrintf("CardReadFile returns 2 containers");
				CONTAINERMAPREC *c2 = (CONTAINERMAPREC *) 
					(t + sizeof(CONTAINERMAPREC));
				wcsncpy((PWCHAR ) c2->GuidInfo,sigGuid,sizeof(c2->GuidInfo) / 2);
				c2->Flags = 1; // 1 valid 
				c2->ui16KeyExchangeKeySize = 0;
				c2->ui16SigKeySize = 1024;
				}
			*ppbData = originalT;
			*pcbData = sz;
			return ret(E_OK);
			}
		}
		dbgPrintf("returning E_NOFILE");
		return ret(E_NOFILE);
	}


DWORD WINAPI
CardWriteFile(
    __in                     PCARD_DATA  pCardData,
    __in                     LPSTR       pszDirectoryName,
    __in                     LPSTR       pszFileName,
    __in                     DWORD       dwFlags,
    __in_bcount(cbData)      PBYTE       pbData,
    __in                     DWORD       cbData){
	if (!pCardData) return ret(E_PARAM);
	dbgPrintf("CardWriteFile pszDirectoryName=%s, pszFileName=%s, dwFlags=0x%08X, cbData=%u"
		,NULLSTR(pszDirectoryName),NULLSTR(pszFileName),dwFlags,cbData);
	return ret(E_UNSUPPORTED);
}


DWORD WINAPI
CardQueryFreeSpace(
    __in      PCARD_DATA  pCardData,
    __in      DWORD       dwFlags,
    __in      PCARD_FREE_SPACE_INFO pCardFreeSpaceInfo) {
	if (!pCardData) 
		return ret(E_PARAM);
	if (!pCardFreeSpaceInfo) 
		return ret(E_PARAM);
	dbgPrintf("CardWriteFile dwFlags=0x%08X, dwVersion=%u"
		, dwFlags , pCardFreeSpaceInfo->dwVersion );
	if (dwFlags) 
		return ret(E_PARAM);

	if (pCardFreeSpaceInfo->dwVersion != CARD_FREE_SPACE_INFO_CURRENT_VERSION  
	 && pCardFreeSpaceInfo->dwVersion != 0) 
		return ret(E_REVISION);

	pCardFreeSpaceInfo->dwVersion = CARD_FREE_SPACE_INFO_CURRENT_VERSION;
	pCardFreeSpaceInfo->dwBytesAvailable = 0;
	pCardFreeSpaceInfo->dwKeyContainersAvailable = 0;
	pCardFreeSpaceInfo->dwMaxKeyContainers = 2;
	return ret(E_OK);
	}

DWORD WINAPI
CardQueryKeySizes(
    __in      PCARD_DATA  pCardData,
    __in      DWORD       dwKeySpec,
    __in      DWORD       dwFlags,
    __in      PCARD_KEY_SIZES pKeySizes){
	if (!pCardData)
		return ret(E_PARAM);
	if (!pKeySizes) {
		dbgPrintf("CardQueryKeySizes NULL pKeySizes");
		return ret(E_PARAM);
		}
	dbgPrintf("CardQueryKeySizes dwKeySpec=%u, dwFlags=0x%08X, dwVersion=%u"
		,dwKeySpec,dwFlags,pKeySizes->dwVersion );
	if (dwFlags) return  ret(E_PARAM);
	if (dwKeySpec > 8 || dwKeySpec == 0 ) return ret(E_PARAM);
	if (dwKeySpec != AT_SIGNATURE && 
		dwKeySpec != AT_KEYEXCHANGE ) {	
		return ret(E_UNSUPPORTED);
		}
	if (pKeySizes->dwVersion > CARD_KEY_SIZES_CURRENT_VERSION) 
		return ret(E_REVISION);

//	pKeySizes->dwVersion = CARD_KEY_SIZES_CURRENT_VERSION;
	pKeySizes->dwDefaultBitlen = 1024;
	pKeySizes->dwMaximumBitlen = 1024;
	pKeySizes->dwMinimumBitlen = 1024;
	pKeySizes->dwIncrementalBitlen = 0;
	return ret(E_OK);
}

DWORD WINAPI
CardRSADecrypt(
    __in        PCARD_DATA              pCardData,
    __inout     PCARD_RSA_DECRYPT_INFO  pInfo){
	if (!pCardData) return ret(E_PARAM);
	if (!pInfo) return ret(E_PARAM);
	dbgPrintf("CardRSADecrypt dwVersion=%u, bContainerIndex=%u, dwKeySpec=%u"
		,pInfo->dwVersion,pInfo->bContainerIndex , pInfo->dwKeySpec);

	if (pInfo->dwVersion != CARD_RSA_KEY_DECRYPT_INFO_CURRENT_VERSION) 
		return ret(E_REVISION);

	if (pInfo->dwKeySpec > 8 || pInfo->dwKeySpec == 0 ) 
		return ret(E_PARAM);
	if (pInfo->dwKeySpec != AT_SIGNATURE && pInfo->dwKeySpec != AT_KEYEXCHANGE ) {	
		return ret(E_PARAM);
		}
	if (pInfo->bContainerIndex != AUTH_CONTAINER_INDEX && 
		pInfo->bContainerIndex != SIGN_CONTAINER_INDEX )
		return ret(E_NOCONTAINER);

	if (pInfo->bContainerIndex == AUTH_CONTAINER_INDEX ) {
		if (pInfo->dwKeySpec != AT_KEYEXCHANGE) return ret(E_PARAM);
		}
	else 
		if (pInfo->dwKeySpec != AT_SIGNATURE) return ret(E_PARAM);

	if (!pInfo->pbData)
		return ret(E_PARAM);
	if (pInfo->cbData < 1024 / 8) 
		return ret(E_SCBUFFER);

#ifdef DEBUG 
	dbgPrintf("pInfo->bContainerIndex=%i, pInfo->dwKeySpec=%i, pInfo->dwVersion=%i",
		pInfo->bContainerIndex, pInfo->dwKeySpec, pInfo->dwVersion);
	for(unsigned int j=0;j < pInfo->cbData;j++) {
		if (!(j % 16)) fprintf(debugfp,"\n");
		fprintf(debugfp,"0x%02X, ",pInfo->pbData[j]);
		}
	fprintf(debugfp,"\n");
#endif

	ByteVec reply;
	try {
		ByteVec cipher(pInfo->pbData ,pInfo->pbData + pInfo->cbData ); 
		reverse(cipher.begin(),cipher.end());
		PCSCManager mgr(pCardData->hSCardCtx);
		EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
		scard.setLogging(&logStream);
		reply = scard.RSADecrypt(/*authPin,*/cipher);
	} catch (AuthError &err) {
		dbgPrintf("SCError exception thrown: %s", err.what());
		if (err.SW1 == 0x69 && err.SW2 == 0x88 ) 
			return ret(E_BADDATA); //fyr digidoc
		else
			return ret(E_NEEDSAUTH);
	} catch (CardError & err) {
		dbgPrintf("cardError exception thrown: %s SW1=0x%02X SW2=0x%02X", err.what(),
			err.SW1 , err.SW2 );
		if (err.SW1 == 0x64 && err.SW2 == 0 ) 
			return ret(E_BADDATA); //fyr digidoc
		else
			return ret(E_INTERNAL);
	} catch (runtime_error & ex) {
		dbgPrintf("runtime_error exception thrown: %s", ex.what());
		return ret(E_INTERNAL);
		}
	//E_NEEDSAUTH

	//our data comes out in wrong order and needs to be repadded
	int psLen = (int )(128 - reply.size() - 3);
	ByteVec pB(0);

	srand((unsigned int)time(0));
	reverse(reply.begin(),reply.end());
	pB.insert(pB.end(),reply.begin(),reply.end());
	pB.push_back(0);
	for (;psLen > 0;psLen --) {
		BYTE br;
		while(0 == (br = LOBYTE(rand())));
		pB.push_back( br );
		}
	pB.push_back(2);
	pB.push_back(0);

/*	CARD_SIGNING_INFO temp;
	BCRYPT_PKCS1_PADDING_INFO pinf;
	pinf.pszAlgId = L"SHA1";

	ZeroMemory(&temp,sizeof(temp));
	DWORD padLen;
	PBYTE padBuf;
//		BCRYPT_PKCS1_PADDING_INFO *pinf = (BCRYPT_PKCS1_PADDING_INFO *)pInfo->pPaddingInfo;
	temp.dwVersion = CARD_SIGNING_INFO_BASIC_VERSION;
	temp.pbSignedData = &reply[0];
	temp.cbSignedData = reply.size();
	temp.aiHashAlg = ALG_CLASS_ALL | ALG_TYPE_RSA | ALG_SID_RSA_PKCS; //CALG_RSA_SIGN;
	temp.dwPaddingType = CARD_PADDING_PKCS1;
	temp.pPaddingInfo = &pinf;

	DWORD code = pCardData->pfnCspPadData(&temp,128, &padLen, &padBuf);
	ByteVec pB0(padBuf,padBuf + padLen); */

	if (pInfo->cbData != pB.size()) {
		dbgPrintf("invalid condition in CardRSADecrypt");
		return ret(E_INTERNAL);
		}
	CopyMemory(pInfo->pbData,&pB[0],pB.size());

	return ret(E_OK);
	}

DWORD WINAPI
CardSignData(
    __in      PCARD_DATA          pCardData,
    __in      PCARD_SIGNING_INFO  pInfo){
	if (!pCardData) return ret(E_PARAM);
	if (!pInfo) return ret(E_PARAM);

	dbgPrintf("CardSignData dwVersion=%u, bContainerIndex=%u, dwKeySpec=%u"
		", dwSigningFlags=0x%08X, aiHashAlg=0x%08X, cbData=%u"
		,pInfo->dwVersion,pInfo->bContainerIndex , pInfo->dwKeySpec
		,pInfo->dwSigningFlags,pInfo->aiHashAlg, pInfo->cbData  );

	ALG_ID hashAlg = pInfo->aiHashAlg;

	if (!pInfo->pbData) return ret(E_PARAM);
	if (pInfo->bContainerIndex != AUTH_CONTAINER_INDEX &&
		pInfo->bContainerIndex != SIGN_CONTAINER_INDEX)
		return ret(E_NOCONTAINER);
	if (pInfo->dwVersion > 1) {
		dbgPrintf("CardSignData(3) dwPaddingType=%u",pInfo->dwPaddingType);
		}

	if (pInfo->dwVersion != 1 && pInfo->dwVersion != 2)  {
		dbgPrintf("unsupported version ");
		return ret(E_REVISION);
		}
	if (pInfo->dwKeySpec != AT_KEYEXCHANGE && pInfo->dwKeySpec != AT_SIGNATURE ) {
		dbgPrintf("unsupported dwKeySpec");
		return ret(E_PARAM);
		}

	DWORD dwFlagMask = CARD_PADDING_INFO_PRESENT | CARD_BUFFER_SIZE_ONLY
		| CARD_PADDING_NONE | CARD_PADDING_PKCS1 | CARD_PADDING_PSS;
	if (pInfo->dwSigningFlags & (~dwFlagMask)) {
		dbgPrintf("bogus dwSigningFlags");
		return ret(E_PARAM);
		}

	if (CARD_PADDING_INFO_PRESENT & pInfo->dwSigningFlags) {
		if (CARD_PADDING_PKCS1 != pInfo->dwPaddingType) {
			dbgPrintf("unsupported paddingtype");
			return ret(E_UNSUPPORTED);
			}
		BCRYPT_PKCS1_PADDING_INFO_adhoc *pinf = (BCRYPT_PKCS1_PADDING_INFO_adhoc *)pInfo->pPaddingInfo;
		if (!pinf->pszAlgId) 
			hashAlg = CALG_SSL3_SHAMD5;
		else {
			if (pinf->pszAlgId == wstring(L"MD5"))  hashAlg = CALG_MD5;
			if (pinf->pszAlgId == wstring(L"SHA1"))  hashAlg = CALG_SHA1;
			}
		}
	else {
		if (GET_ALG_CLASS(hashAlg) != ALG_CLASS_HASH) {
			dbgPrintf("bogus aiHashAlg");
			return ret(E_PARAM);
			}
		if (hashAlg !=0 && hashAlg != CALG_SSL3_SHAMD5 &&
			hashAlg != CALG_SHA1 && hashAlg != CALG_MD5
			) {
			dbgPrintf("unsupported aiHashAlg");
			return ret(E_UNSUPPORTED);
			}
		}

	if (pInfo->bContainerIndex != AUTH_CONTAINER_INDEX &&
		pInfo->bContainerIndex != SIGN_CONTAINER_INDEX ) {
		dbgPrintf("invalid container index");
		return ret(E_NOCONTAINER);
		}

	DWORD sz = 1024/ 8;
	ByteVec reply;

	ByteVec hash(pInfo->pbData ,pInfo->pbData + pInfo->cbData ); 

	bool withOID = (pInfo->dwSigningFlags & CRYPT_NOHASHOID) ? false : true;
	try {
		PCSCManager mgr(pCardData->hSCardCtx);
		EstEidCard scard(mgr,mgr.connect(pCardData->hScard));
		scard.setLogging(&logStream);
		if (hashAlg == CALG_SHA1)
			reply = scard.calcSignSHA1(hash,pInfo->bContainerIndex == AUTH_CONTAINER_INDEX ? 
				EstEidCard::AUTH : EstEidCard::SIGN ,withOID);
		if (hashAlg == CALG_MD5)
			reply = scard.calcSignMD5(hash,pInfo->bContainerIndex == AUTH_CONTAINER_INDEX ? 
				EstEidCard::AUTH : EstEidCard::SIGN,withOID);
		if (hashAlg == CALG_SSL3_SHAMD5 || hashAlg == 0) {
			if (pInfo->bContainerIndex != AUTH_CONTAINER_INDEX) {
				dbgPrintf("SSL requested with signature key");
				return ret(E_UNSUPPORTED);
				}
			reply = scard.calcSSL(hash);
			}
		if (reply.size() == 0) {
			dbgPrintf("no function to call, hashAlg 0x%08X, container %d",hashAlg,pInfo->bContainerIndex);
			return ret(E_UNSUPPORTED);
			}
	} catch (AuthError &err) {
		dbgPrintf("SCError exception thrown: %s", err.what());
		return ret(E_NEEDSAUTH);
	} catch (runtime_error &ex) {
		dbgPrintf("runtime_error exception thrown:", ex.what());
		return ret(E_INTERNAL);
		}
	
	reverse(reply.begin(),reply.end());

	pInfo->cbSignedData = (DWORD) reply.size();
	if (!(pInfo->dwSigningFlags & CARD_BUFFER_SIZE_ONLY)) {
		pInfo->pbSignedData = (PBYTE)(*pCardData->pfnCspAlloc)(reply.size());
		if (!pInfo->pbSignedData) return ret(E_MEMORY);
		CopyMemory(pInfo->pbSignedData,&reply[0],reply.size());
		}
	return ret(E_OK);
}

DWORD WINAPI
CardDeauthenticate(
    __in    PCARD_DATA  pCardData,
    __in    LPWSTR      pwszUserId,
    __in    DWORD       dwFlags
	){
	dbgPrintf("CardDeauthenticate:dummy");
	return ret(E_UNSUPPORTED);
}

DWORD WINAPI CardDeauthenticateEx(
    __in   PCARD_DATA                             pCardData,
    __in   PIN_SET                                PinId,
	__in   DWORD                                  dwFlags
	) {
	dbgPrintf("CardDeauthenticateEx:dummy");
	return ret(E_UNSUPPORTED);
}
