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

#include "precompiled.h"
#include "Csp.h"

extern Csp *csp;

extern "C" {

CSP_API HRESULT DllUnregisterServer(void) {
	return csp->DllUnregisterServer();
	}
CSP_API HRESULT DllRegisterServer(void) {
	return csp->DllRegisterServer();
}

BOOL CSP_API WINAPI 
CPAcquireContext(
    OUT HCRYPTPROV *phProv,
    IN  LPCSTR szContainer,
    IN  DWORD dwFlags,
    IN  PVTableProvStruc pVTable){
	return csp->CPAcquireContext(phProv,szContainer,dwFlags,pVTable);
}

CSP_API BOOL WINAPI
CPAcquireContextW(
    OUT HCRYPTPROV *phProv,
    IN  LPCWSTR szContainer,
    IN  DWORD dwFlags,
    IN  PVTableProvStrucW pVTable){
	return csp->CPAcquireContextW(phProv,szContainer,
		dwFlags,pVTable);
}

CSP_API BOOL WINAPI
CPReleaseContext(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwFlags){
	return csp->CPReleaseContext(hProv,dwFlags);
}

CSP_API BOOL WINAPI
CPGenKey(
    IN  HCRYPTPROV hProv,
    IN  ALG_ID Algid,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey){
	return csp->CPGenKey(hProv,Algid,dwFlags,phKey);
}

CSP_API BOOL WINAPI
CPDeriveKey(
    IN  HCRYPTPROV hProv,
    IN  ALG_ID Algid,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey){
	return csp->CPDeriveKey(hProv,Algid,hHash,
		dwFlags,phKey);
}

CSP_API BOOL WINAPI
CPDestroyKey(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey){
	return csp->CPDestroyKey(hProv,hKey);
}

CSP_API BOOL WINAPI
CPSetKeyParam(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwParam,
    IN  CONST BYTE *pbData,
    IN  DWORD dwFlags){
	return csp->CPSetKeyParam(hProv,hKey,dwParam,
		pbData,dwFlags);
}

CSP_API BOOL WINAPI
CPGetKeyParam(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwParam,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD dwFlags){
	return csp->CPGetKeyParam(hProv,hKey,dwParam,pbData,
		pcbDataLen,dwFlags);
}

CSP_API BOOL WINAPI
CPSetProvParam(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwParam,
    IN  CONST BYTE *pbData,
    IN  DWORD dwFlags){
	return csp->CPSetProvParam(hProv,dwParam,pbData,dwFlags);
}

CSP_API BOOL WINAPI
CPGetProvParam(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwParam,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD dwFlags){
	return csp->CPGetProvParam(hProv,dwParam,pbData,pcbDataLen,dwFlags);
}

CSP_API BOOL WINAPI
CPSetHashParam(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwParam,
    IN  CONST BYTE *pbData,
    IN  DWORD dwFlags){
	return csp->CPSetHashParam(hProv,hHash,dwParam,pbData,dwFlags);
}


CSP_API BOOL WINAPI
CPGetHashParam(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwParam,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD dwFlags){
	return csp->CPGetHashParam(hProv,hHash,dwParam,
		pbData,pcbDataLen,dwFlags);
}

CSP_API BOOL WINAPI
CPExportKey(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTKEY hPubKey,
    IN  DWORD dwBlobType,
    IN  DWORD dwFlags,
    OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen){
	return csp->CPExportKey(hProv,hKey,hPubKey,dwBlobType,
		dwFlags,pbData,pcbDataLen);
}

CSP_API BOOL WINAPI
CPImportKey(
    IN  HCRYPTPROV hProv,
    IN  CONST BYTE *pbData,
    IN  DWORD cbDataLen,
    IN  HCRYPTKEY hPubKey,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey){
	return csp->CPImportKey(hProv,pbData,cbDataLen,hPubKey,dwFlags,phKey);
}

CSP_API BOOL WINAPI
CPEncrypt(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTHASH hHash,
    IN  BOOL fFinal,
    IN  DWORD dwFlags,
    IN OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen,
    IN  DWORD cbBufLen){
	return csp->CPEncrypt(hProv,hKey,hHash,fFinal,
		dwFlags,pbData,pcbDataLen,cbBufLen);
}

CSP_API BOOL WINAPI
CPDecrypt(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  HCRYPTHASH hHash,
    IN  BOOL fFinal,
    IN  DWORD dwFlags,
    IN OUT LPBYTE pbData,
    IN OUT LPDWORD pcbDataLen){
	return csp->CPDecrypt(hProv,hKey,hHash,fFinal,
		dwFlags,pbData,pcbDataLen);
}

CSP_API BOOL WINAPI
CPCreateHash(
    IN  HCRYPTPROV hProv,
    IN  ALG_ID Algid,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwFlags,
    OUT HCRYPTHASH *phHash){
	return csp->CPCreateHash(hProv,Algid,hKey,dwFlags,phHash);
}

CSP_API BOOL WINAPI
CPHashData(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  CONST BYTE *pbData,
    IN  DWORD cbDataLen,
    IN  DWORD dwFlags){
	return csp->CPHashData(hProv,hHash,pbData,cbDataLen,dwFlags);
}

CSP_API BOOL WINAPI
CPHashSessionKey(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  HCRYPTKEY hKey,
    IN  DWORD dwFlags){
	return csp->CPHashSessionKey(hProv,hHash,hKey,dwFlags);
}

CSP_API BOOL WINAPI
CPSignHash(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  DWORD dwKeySpec,
    IN  LPCWSTR szDescription,
    IN  DWORD dwFlags,
    OUT LPBYTE pbSignature,
    IN OUT LPDWORD pcbSigLen){
	return csp->CPSignHash(hProv,hHash,dwKeySpec,
		szDescription,dwFlags,pbSignature,pcbSigLen);
}

CSP_API BOOL WINAPI
CPDestroyHash(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash){
	return csp->CPDestroyHash(hProv,hHash);
}

CSP_API BOOL WINAPI
CPVerifySignature(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  CONST BYTE *pbSignature,
    IN  DWORD cbSigLen,
    IN  HCRYPTKEY hPubKey,
    IN  LPCWSTR szDescription,
    IN  DWORD dwFlags){
	return csp->CPVerifySignature(hProv,hHash,
		pbSignature,cbSigLen,hPubKey,szDescription,dwFlags);
}

CSP_API BOOL WINAPI
CPGenRandom(
    IN  HCRYPTPROV hProv,
    IN  DWORD cbLen,
    OUT LPBYTE pbBuffer){
	return csp->CPGenRandom(hProv,cbLen,pbBuffer);
}

CSP_API BOOL WINAPI
CPGetUserKey(
    IN  HCRYPTPROV hProv,
    IN  DWORD dwKeySpec,
    OUT HCRYPTKEY *phUserKey){
	return csp->CPGetUserKey(hProv,dwKeySpec,phUserKey);
}

CSP_API BOOL WINAPI
CPDuplicateHash(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTHASH hHash,
    IN  LPDWORD pdwReserved,
    IN  DWORD dwFlags,
    OUT HCRYPTHASH *phHash){
	return csp->CPDuplicateHash(hProv,hHash,pdwReserved,dwFlags,phHash);
}

CSP_API BOOL WINAPI
CPDuplicateKey(
    IN  HCRYPTPROV hProv,
    IN  HCRYPTKEY hKey,
    IN  LPDWORD pdwReserved,
    IN  DWORD dwFlags,
    OUT HCRYPTKEY *phKey){
	return csp->CPDuplicateKey(hProv,hKey,pdwReserved,dwFlags,phKey);
}

}
