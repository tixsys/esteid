/*!
	\file		Csp_Hash.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 09:51:33 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 472 $

#include "precompiled.h"
#include "Csp.h"

BOOL Csp::CPCreateHash(
		IN  HCRYPTPROV hProv,
		IN  ALG_ID Algid,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwFlags,
		OUT HCRYPTHASH *phHash){
	DECL_RET(ret);
	try {
		CSPContext *it = *findContext(hProv);
		CSPHashContext * newHash = it->createHashContext();
		newHash->m_wrapHash = new WrapHash(
			*it->m_wrapCsp
			,Algid,hKey,dwFlags);
		newHash->m_hashId = getNextHandle();
		newHash->m_algId = Algid;
		it->m_hashes.push_back(newHash);
		*phHash = newHash->m_hashId;
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPHashData(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  CONST BYTE *pbData,
		IN  DWORD cbDataLen,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		CSPContext *it = *findContext(hProv);
		CSPHashContext *hash = *it->findHashContext(hHash);
		CryptHashData(*hash->m_wrapHash,pbData,cbDataLen,dwFlags);
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPHashSessionKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPSignHash(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwKeySpec,
		IN  LPCWSTR szDescription,
		IN  DWORD dwFlags,
		OUT LPBYTE pbSignature,
		IN OUT LPDWORD pcbSigLen){
	DECL_RET(ret);
	try { //need to get a key
		CSPContext * it = *findContext(hProv);
		CSPHashContext * hash = *it->findHashContext(hHash);
		packData dat(pbSignature,pcbSigLen);
		if (hash->signature.empty()) {
			DWORD bufSz = 0;
			CryptGetHashParam(*hash->m_wrapHash,HP_HASHVAL, NULL, &bufSz, 0);
			std::vector<BYTE> buffer(bufSz,'\0');
			CryptGetHashParam(*hash->m_wrapHash,HP_HASHVAL,&buffer[0],&bufSz,0);
			hash->signature = hash->sign(buffer,dwKeySpec);
			}
		dat.setValue(hash->signature);
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPDestroyHash(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		it->m_hashes.erase(it->findHashContext(hHash));
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPVerifySignature(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  CONST BYTE *pbSignature,
		IN  DWORD cbSigLen,
		IN  HCRYPTKEY hPubKey,
		IN  LPCWSTR szDescription,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPSetHashParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwParam,
		IN  CONST BYTE *pbData,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		CSPHashContext * hash = *it->findHashContext(hHash);
		CryptSetHashParam( *hash->m_wrapHash,dwParam, pbData,dwFlags);
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPGetHashParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwParam,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		//fRes=CryptGetHashParam(sha1,HP_HASHVAL, hash,hashLen ,0);
		CSPContext * it = *findContext(hProv);
		CSPHashContext * hash = *it->findHashContext(hHash);
		CryptGetHashParam( *hash->m_wrapHash,dwParam, pbData, pcbDataLen, dwFlags);
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPDuplicateHash(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTHASH hHash,
		IN  LPDWORD pdwReserved,
		IN  DWORD dwFlags,
		OUT HCRYPTHASH *phHash){
	DECL_RET(ret);
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

