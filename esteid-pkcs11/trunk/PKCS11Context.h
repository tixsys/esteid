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

#pragma once

#include "pkcs11.h"

#ifndef CK_DECLARE_FUNCTION
#define CK_DECLARE_FUNCTION CK_DEFINE_FUNCTION
#endif

#include <vector>

#define NOCOPY(ptype) private: ptype(const ptype &o); const ptype &operator=(const ptype &o)

class PKCS11ContextPriv;

class PKCS11Context
{
	PKCS11ContextPriv *d; //pimpl idiom
	void refreshReaders();
	CK_SESSION_HANDLE getNextSessionHandle();
public:
	PKCS11Context(void);
	~PKCS11Context(void);
#undef CK_EXPORT_SPEC 
#define CK_EXPORT_SPEC 
	CK_DEFINE_FUNCTION(CK_RV,C_GetInfo(CK_INFO_PTR   pInfo  ));
	CK_DEFINE_FUNCTION(CK_RV,C_GetSlotList(
		CK_BBOOL       tokenPresent,  /* only slots with tokens? */
		CK_SLOT_ID_PTR pSlotList,     /* receives array of slot IDs */
		CK_ULONG_PTR   pulCount       /* receives number of slots */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_GetTokenInfo(
		CK_SLOT_ID        slotID,  /* ID of the token's slot */
		CK_TOKEN_INFO_PTR pInfo    /* receives the token information */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_GetSlotInfo(
		CK_SLOT_ID       slotID,  /* the ID of the slot */
		CK_SLOT_INFO_PTR pInfo    /* receives the slot information */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_GetMechanismList(
		CK_SLOT_ID            slotID,          /* ID of token's slot */
		CK_MECHANISM_TYPE_PTR pMechanismList,  /* gets mech. array */
		CK_ULONG_PTR          pulCount         /* gets # of mechs. */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_GetMechanismInfo(
		CK_SLOT_ID            slotID,  /* ID of the token's slot */
		CK_MECHANISM_TYPE     type,    /* type of mechanism */
		CK_MECHANISM_INFO_PTR pInfo    /* receives mechanism info */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_InitToken(
		CK_SLOT_ID      slotID,    /* ID of the token's slot */
		CK_UTF8CHAR_PTR pPin,      /* the SO's initial PIN */
		CK_ULONG        ulPinLen,  /* length in bytes of the PIN */
		CK_UTF8CHAR_PTR pLabel     /* 32-byte token label (blank padded) */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_OpenSession(
		CK_SLOT_ID            slotID,        /* the slot's ID */
		CK_FLAGS              flags,         /* from CK_SESSION_INFO */
		CK_VOID_PTR           pApplication,  /* passed to callback */
		CK_NOTIFY             Notify,        /* callback function */
		CK_SESSION_HANDLE_PTR phSession      /* gets session handle */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_GetSessionInfo(
		CK_SESSION_HANDLE   hSession,  /* the session's handle */
		CK_SESSION_INFO_PTR pInfo
		));
	CK_DEFINE_FUNCTION(CK_RV,C_FindObjectsInit(
		CK_SESSION_HANDLE hSession,   /* the session's handle */
		CK_ATTRIBUTE_PTR  pTemplate,  /* attribute values to match */
		CK_ULONG          ulCount     /* attrs in search template */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_FindObjects(
		CK_SESSION_HANDLE    hSession,          /* session's handle */
		CK_OBJECT_HANDLE_PTR phObject,          /* gets obj. handles */
		CK_ULONG             ulMaxObjectCount,  /* max handles to get */
		CK_ULONG_PTR         pulObjectCount     /* actual # returned */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_FindObjectsFinal(
		CK_SESSION_HANDLE hSession  /* the session's handle */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_Login(
		CK_SESSION_HANDLE hSession,  /* the session's handle */
		CK_USER_TYPE      userType,  /* the user type */
		CK_UTF8CHAR_PTR   pPin,      /* the user's PIN */
		CK_ULONG          ulPinLen   /* the length of the PIN */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_GetAttributeValue(
		CK_SESSION_HANDLE hSession,   /* the session's handle */
		CK_OBJECT_HANDLE  hObject,    /* the object's handle */
		CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attrs; gets vals */
		CK_ULONG          ulCount     /* attributes in template */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_SignInit(
		CK_SESSION_HANDLE hSession,    /* the session's handle */
		CK_MECHANISM_PTR  pMechanism,  /* the signature mechanism */
		CK_OBJECT_HANDLE  hKey         /* handle of signature key */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_Sign(
		CK_SESSION_HANDLE hSession,        /* the session's handle */
		CK_BYTE_PTR       pData,           /* the data to sign */
		CK_ULONG          ulDataLen,       /* count of bytes to sign */
		CK_BYTE_PTR       pSignature,      /* gets the signature */
		CK_ULONG_PTR      pulSignatureLen  /* gets signature length */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_SignUpdate(
		CK_SESSION_HANDLE hSession,  /* the session's handle */
		CK_BYTE_PTR       pPart,     /* the data to sign */
		CK_ULONG          ulPartLen  /* count of bytes to sign */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_SignFinal(
		CK_SESSION_HANDLE hSession,        /* the session's handle */
		CK_BYTE_PTR       pSignature,      /* gets the signature */
		CK_ULONG_PTR      pulSignatureLen  /* gets signature length */
		));
	CK_DEFINE_FUNCTION(CK_RV,C_Logout(
		CK_SESSION_HANDLE hSession  /* the session's handle */ ));
};

