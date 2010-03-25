/*
 * esteid-pkcs11 - PKCS#11 module for Estonian EID card
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// esteidpkcs11.cpp : Defines the entry point for the DLL application.
//

#include "precompiled.h"
#include "PKCS11Context.h"

#ifdef _WIN32
#include <crtdbg.h>
#endif

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#if defined(_WIN32)
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if defined(DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF |_CRTDBG_CHECK_ALWAYS_DF |
				_CRTDBG_CHECK_CRT_DF |_CRTDBG_DELAY_FREE_MEM_DF );
#endif
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif

#ifdef _MANAGED
#pragma managed(pop)
#endif

PKCS11Context *ctx = NULL;

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_Initialize(CK_VOID_PTR pInitArgs)) {
	try {
		ctx = new PKCS11Context();
	} catch (std::runtime_error &) {
		return CKR_FUNCTION_FAILED;
		}
	return CKR_OK;
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_Finalize(CK_VOID_PTR   pReserved)) {
	if (ctx) 
		delete ctx;
	ctx = NULL;
	return CKR_OK;
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetInfo(CK_INFO_PTR   pInfo  )) {
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetInfo(pInfo);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetSlotList(
		CK_BBOOL       tokenPresent,  /* only slots with tokens? */
		CK_SLOT_ID_PTR pSlotList,     /* receives array of slot IDs */
		CK_ULONG_PTR   pulCount)) {   /* receives number of slots */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetSlotList(tokenPresent,pSlotList,pulCount);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetTokenInfo(
		CK_SLOT_ID        slotID,  /* ID of the token's slot */
		CK_TOKEN_INFO_PTR pInfo)) {/* receives the token information */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetTokenInfo(slotID,pInfo);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetSlotInfo(
		CK_SLOT_ID       slotID,  /* the ID of the slot */
		CK_SLOT_INFO_PTR pInfo)) {/* receives the slot information */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetSlotInfo(slotID,pInfo);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetMechanismList(
		CK_SLOT_ID            slotID,          /* ID of token's slot */
		CK_MECHANISM_TYPE_PTR pMechanismList,  /* gets mech. array */
		CK_ULONG_PTR          pulCount)) {     /* gets # of mechs. */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetMechanismList(slotID,pMechanismList,pulCount);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetMechanismInfo(
		CK_SLOT_ID            slotID,  /* ID of the token's slot */
		CK_MECHANISM_TYPE     type,    /* type of mechanism */
		CK_MECHANISM_INFO_PTR pInfo)) {/* receives mechanism info */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetMechanismInfo(slotID,type,pInfo);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_InitToken(
		CK_SLOT_ID      slotID,    /* ID of the token's slot */
		CK_UTF8CHAR_PTR pPin,      /* the SO's initial PIN */
		CK_ULONG        ulPinLen,  /* length in bytes of the PIN */
		CK_UTF8CHAR_PTR pLabel)) { /* 32-byte token label (blank padded) */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_InitToken(slotID,pPin,ulPinLen,pLabel);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_OpenSession(
		CK_SLOT_ID            slotID,        /* the slot's ID */
		CK_FLAGS              flags,         /* from CK_SESSION_INFO */
		CK_VOID_PTR           pApplication,  /* passed to callback */
		CK_NOTIFY             Notify,        /* callback function */
		CK_SESSION_HANDLE_PTR phSession)) {  /* gets session handle */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_OpenSession(slotID,flags,pApplication,Notify,phSession);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_CloseSession(
		CK_SESSION_HANDLE hSession)) {  /* the session's handle */
	return CKR_OK;
}
extern "C" CK_DECLARE_FUNCTION(CK_RV,C_CloseAllSessions(
		CK_SLOT_ID     slotID )) {  /* the token's slot */
	return CKR_OK;
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetSessionInfo(
		CK_SESSION_HANDLE   hSession,  /* the session's handle */
		CK_SESSION_INFO_PTR pInfo)) {  /* receives session info */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetSessionInfo(hSession,pInfo);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_FindObjectsInit(
		CK_SESSION_HANDLE hSession,   /* the session's handle */
		CK_ATTRIBUTE_PTR  pTemplate,  /* attribute values to match */
		CK_ULONG          ulCount)) { /* attrs in search template */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_FindObjectsInit(hSession,pTemplate,ulCount);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_FindObjects(
		CK_SESSION_HANDLE    hSession,          /* session's handle */
		CK_OBJECT_HANDLE_PTR phObject,          /* gets obj. handles */
		CK_ULONG             ulMaxObjectCount,  /* max handles to get */
		CK_ULONG_PTR         pulObjectCount)) { /* actual # returned */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_FindObjects(hSession,phObject,ulMaxObjectCount,pulObjectCount);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_FindObjectsFinal(
		CK_SESSION_HANDLE hSession  /* the session's handle */)) {
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_FindObjectsFinal(hSession);
	}

extern "C"  CK_DECLARE_FUNCTION(CK_RV,C_Login(
		CK_SESSION_HANDLE hSession,  /* the session's handle */
		CK_USER_TYPE      userType,  /* the user type */
		CK_UTF8CHAR_PTR   pPin,      /* the user's PIN */
		CK_ULONG          ulPinLen)){/* the length of the PIN */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_Login(hSession,userType,pPin,ulPinLen);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_Logout(
		CK_SESSION_HANDLE hSession)) { /* the session's handle */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_Logout(hSession);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetAttributeValue(
	CK_SESSION_HANDLE hSession,   /* the session's handle */
	CK_OBJECT_HANDLE  hObject,    /* the object's handle */
	CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attrs; gets vals */
	CK_ULONG          ulCount)) { /* attributes in template */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_GetAttributeValue(hSession,hObject,pTemplate,ulCount);
	}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_SignInit(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_MECHANISM_PTR  pMechanism,  /* the signature mechanism */
	CK_OBJECT_HANDLE  hKey         /* handle of signature key */
	)) {
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_SignInit(hSession,pMechanism,hKey);
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_Sign(
	CK_SESSION_HANDLE hSession,        /* the session's handle */
	CK_BYTE_PTR       pData,           /* the data to sign */
	CK_ULONG          ulDataLen,       /* count of bytes to sign */
	CK_BYTE_PTR       pSignature,      /* gets the signature */
	CK_ULONG_PTR      pulSignatureLen  /* gets signature length */
	)) {
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_Sign(hSession,pData,ulDataLen,pSignature,pulSignatureLen);
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_SignUpdate(
		CK_SESSION_HANDLE hSession,  /* the session's handle */
		CK_BYTE_PTR       pPart,     /* the data to sign */
		CK_ULONG          ulPartLen  /* count of bytes to sign */
		)) {
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_SignUpdate(hSession,pPart,ulPartLen);
}
extern "C" CK_DECLARE_FUNCTION(CK_RV,C_SignFinal(
		CK_SESSION_HANDLE hSession,        /* the session's handle */
		CK_BYTE_PTR       pSignature,      /* gets the signature */
		CK_ULONG_PTR      pulSignatureLen  /* gets signature length */
		)) {
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return ctx->C_SignFinal(hSession,pSignature,pulSignatureLen);
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_VerifyInit(
	CK_SESSION_HANDLE hSession,    /* the session's handle */
	CK_MECHANISM_PTR  pMechanism,  /* the verification mechanism */
	CK_OBJECT_HANDLE  hKey)) {    /* verification key */ 
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return CKR_FUNCTION_NOT_SUPPORTED;
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_Verify(
	CK_SESSION_HANDLE hSession,       /* the session's handle */
	CK_BYTE_PTR       pData,          /* signed data */
	CK_ULONG          ulDataLen,      /* length of signed data */
	CK_BYTE_PTR       pSignature,     /* signature */
	CK_ULONG          ulSignatureLen)){/* signature length*/
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return CKR_FUNCTION_NOT_SUPPORTED;
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_VerifyUpdate(
	CK_SESSION_HANDLE hSession,  /* the session's handle */
	CK_BYTE_PTR       pPart,     /* signed data */
	CK_ULONG          ulPartLen)){/* length of signed data */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return CKR_FUNCTION_NOT_SUPPORTED;
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_VerifyFinal(
  CK_SESSION_HANDLE hSession,       /* the session's handle */
  CK_BYTE_PTR       pSignature,     /* signature to verify */
  CK_ULONG          ulSignatureLen)){/* signature length */
	if (!ctx) return CKR_CRYPTOKI_NOT_INITIALIZED;
	return CKR_FUNCTION_NOT_SUPPORTED;
}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_SeedRandom(
	CK_SESSION_HANDLE hSession,  /* the session's handle */
	CK_BYTE_PTR       pSeed,     /* the seed material */
	CK_ULONG          ulSeedLen  /* length of seed material */
)) { return CKR_FUNCTION_NOT_SUPPORTED;}

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_WaitForSlotEvent(
	CK_FLAGS flags,        /* blocking/nonblocking flag */
	CK_SLOT_ID_PTR pSlot,  /* location that receives the slot ID */
	CK_VOID_PTR pRserved   /* reserved.  Should be NULL_PTR */
	)) {
	return CKR_GENERAL_ERROR;
	}

static CK_FUNCTION_LIST functions = {
	{2,20},
	C_Initialize,
	C_Finalize,
	C_GetInfo,
	C_GetFunctionList,
	C_GetSlotList,
	C_GetSlotInfo,
	C_GetTokenInfo,
	C_GetMechanismList,
	C_GetMechanismInfo,
	C_InitToken,//C_InitToken
	NULL,//C_InitPIN
	NULL,//C_SetPIN
	C_OpenSession,
	C_CloseSession,
	C_CloseAllSessions,
	C_GetSessionInfo,//C_GetSessionInfo
	NULL,//C_GetOperationState
	NULL,//C_SetOperationState
	C_Login,
	C_Logout,
	NULL,//C_CreateObject
	NULL,//C_CopyObject
	NULL,//C_DestroyObject
	NULL,//C_GetObjectSize
	C_GetAttributeValue,
	NULL,//C_SetAttributeValue
	C_FindObjectsInit,
	C_FindObjects,
	C_FindObjectsFinal,
	NULL,//C_EncryptInit
	NULL,//C_Encrypt
	NULL,//C_EncryptUpdate
	NULL,//C_EncryptFinal
	NULL,//C_DecryptInit
	NULL,//C_Decrypt
	NULL,//C_DecryptUpdate
	NULL,//C_DecryptFinal
	NULL,//C_DigestInit
	NULL,//C_Digest
	NULL,//C_DigestUpdate
	NULL,//C_DigestKey
	NULL,//C_DigestFinal
	C_SignInit,//C_SignInit
	C_Sign,//C_Sign
	C_SignUpdate,//C_SignUpdate
	C_SignFinal,//C_SignFinal
	NULL,//C_SignRecoverInit
	NULL,//C_SignRecover
	C_VerifyInit,//C_VerifyInit
	C_Verify,//C_Verify
	C_VerifyUpdate,//C_VerifyUpdate
	C_VerifyFinal,//C_VerifyFinal
	NULL,//C_VerifyRecoverInit
	NULL,//C_VerifyRecover
	NULL,//C_DigestEncryptUpdate
	NULL,//C_DecryptDigestUpdate
	NULL,//C_SignEncryptUpdate
	NULL,//C_DecryptVerifyUpdate
	NULL,//C_GenerateKey
	NULL,//C_GenerateKeyPair
	NULL,//C_WrapKey
	NULL,//C_UnwrapKey
	NULL,//C_DeriveKey
	C_SeedRandom,//C_SeedRandom
	NULL,//C_GenerateRandom
	NULL,//C_GetFunctionStatus
	NULL,//C_CancelFunction
	C_WaitForSlotEvent
};

extern "C" CK_DECLARE_FUNCTION(CK_RV,C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList))
{
	*ppFunctionList = &functions;
	return CKR_OK;
}

