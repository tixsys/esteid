/*
 * esteid-csp - CSP for Estonian EID card
 *
 * Copyright (C) 2008-2010  Estonian Informatics Centre
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
#include <utility/logger.h>
#include "CSPTypes.h"
#include "CSPErrors.h"

//#define CSP_API __declspec(dllexport)
#define CSP_API 

typedef BOOL (WINAPI *CRYPT_VERIFY_IMAGE_A)(LPCSTR  szImage, CONST BYTE *pbSigData);
typedef BOOL (WINAPI *CRYPT_VERIFY_IMAGE_W)(LPCWSTR szImage, CONST BYTE *pbSigData);
typedef void (*CRYPT_RETURN_HWND)(HWND *phWnd);

typedef struct _VTableProvStruc {
    DWORD                Version;
    CRYPT_VERIFY_IMAGE_A FuncVerifyImage;
    CRYPT_RETURN_HWND    FuncReturnhWnd;
    DWORD                dwProvType;
    BYTE                *pbContextInfo;
    DWORD                cbContextInfo;
    LPSTR                pszProvName;
} VTableProvStruc,      *PVTableProvStruc;

typedef struct _VTableProvStrucW {
    DWORD                Version;
    CRYPT_VERIFY_IMAGE_W FuncVerifyImage;
    CRYPT_RETURN_HWND    FuncReturnhWnd;
    DWORD                dwProvType;
    BYTE                *pbContextInfo;
    DWORD                cbContextInfo;
    LPWSTR               pszProvName;
} VTableProvStrucW,     *PVTableProvStrucW;


class Csp
{
	std::vector<CSPContext *> m_contexts;
	typedef std::vector<CSPContext *>::iterator CSPContextIter;
	DWORD m_nextHandle;
	CSPContextIter findContext(HCRYPTPROV hProv);
	std::vector<PROV_ENUMALGS> m_enumAlgs;
	std::vector<PROV_ENUMALGS_EX> m_enumAlgsEx;
protected:
	logger m_log;
	tstring m_cspName;
	HMODULE m_module;
public:
	Csp(HMODULE module,TCHAR *);
	virtual ~Csp(void);

	virtual CSPContext * createCSPContext() { return new CSPContext(m_log);}
	///CSP type in registry
	virtual DWORD getCSPType() {return PROV_RSA_FULL;}

	virtual HRESULT DllUnregisterServer(void);
	virtual HRESULT DllRegisterServer(void);

	DWORD getNextHandle();
	virtual BOOL CPAcquireContext(
		OUT HCRYPTPROV *phProv,
		IN  LPCSTR szContainer,
		IN  DWORD dwFlags,
		IN  PVTableProvStruc pVTable);

	virtual BOOL CPAcquireContextW(
		OUT HCRYPTPROV *phProv,
		IN  LPCWSTR szContainer,
		IN  DWORD dwFlags,
		IN  PVTableProvStrucW pVTable);

	virtual BOOL CPReleaseContext(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwFlags);

	virtual BOOL CPGenKey(
		IN  HCRYPTPROV hProv,
		IN  ALG_ID Algid,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey);

	virtual BOOL CPDeriveKey(
		IN  HCRYPTPROV hProv,
		IN  ALG_ID Algid,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey);

	virtual BOOL CPDestroyKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey);

	virtual BOOL CPSetKeyParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwParam,
		IN  CONST BYTE *pbData,
		IN  DWORD dwFlags);

	virtual BOOL CPGetKeyParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwParam,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD dwFlags);

	virtual BOOL CPSetProvParam(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwParam,
		IN  CONST BYTE *pbData,
		IN  DWORD dwFlags);

	virtual BOOL CPGetProvParam(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwParam,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD dwFlags);

	virtual BOOL CPSetHashParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwParam,
		IN  CONST BYTE *pbData,
		IN  DWORD dwFlags);

	virtual BOOL CPGetHashParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwParam,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD dwFlags);

	virtual BOOL CPExportKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  HCRYPTKEY hPubKey,
		IN  DWORD dwBlobType,
		IN  DWORD dwFlags,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen);

	virtual BOOL CPImportKey(
		IN  HCRYPTPROV hProv,
		IN  CONST BYTE *pbData,
		IN  DWORD cbDataLen,
		IN  HCRYPTKEY hPubKey,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey);

	virtual BOOL CPEncrypt(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  HCRYPTHASH hHash,
		IN  BOOL fFinal,
		IN  DWORD dwFlags,
		IN OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD cbBufLen);

	virtual BOOL CPDecrypt(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  HCRYPTHASH hHash,
		IN  BOOL fFinal,
		IN  DWORD dwFlags,
		IN OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen);

	virtual BOOL CPCreateHash(
		IN  HCRYPTPROV hProv,
		IN  ALG_ID Algid,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwFlags,
		OUT HCRYPTHASH *phHash);

	virtual BOOL CPHashData(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  CONST BYTE *pbData,
		IN  DWORD cbDataLen,
		IN  DWORD dwFlags);

	virtual BOOL CPHashSessionKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwFlags);

	virtual BOOL CPSignHash(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwKeySpec,
		IN  LPCWSTR szDescription,
		IN  DWORD dwFlags,
		OUT LPBYTE pbSignature,
		IN OUT LPDWORD pcbSigLen);

	virtual BOOL CPDestroyHash(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash);

	virtual BOOL CPVerifySignature(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  CONST BYTE *pbSignature,
		IN  DWORD cbSigLen,
		IN  HCRYPTKEY hPubKey,
		IN  LPCWSTR szDescription,
		IN  DWORD dwFlags);

	virtual BOOL CPGenRandom(
		IN  HCRYPTPROV hProv,
		IN  DWORD cbLen,
		OUT LPBYTE pbBuffer);

	virtual BOOL CPGetUserKey(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwKeySpec,
		OUT HCRYPTKEY *phUserKey);

	virtual BOOL CPDuplicateHash(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  LPDWORD pdwReserved,
		IN  DWORD dwFlags,
		OUT HCRYPTHASH *phHash);

	virtual BOOL CPDuplicateKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  LPDWORD pdwReserved,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey);
};

#define DECL_RET(var) retType var(__FUNCTION__,m_log)
