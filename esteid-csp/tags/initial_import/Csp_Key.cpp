/*!
	\file		Csp_Key.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-18 13:37:51 +0300 (Sat, 18 Apr 2009) $
*/
// Revision $Revision: 258 $

#include "precompiled.h"
#include "Csp.h"
#pragma comment(lib,"crypt32")

BOOL Csp::CPGetUserKey(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwKeySpec,
		OUT HCRYPTKEY *phUserKey){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		m_log << "GetUserKey, " << it->m_containerName << " dwKeySpec:" << dwKeySpec << std::endl;
		CSPKeyContext * key = it->createKeyContext();
		key->setPubkeySpec(dwKeySpec);
		key->m_keyId = getNextHandle();
		it->m_keys.push_back(key);
		*phUserKey = key->m_keyId;
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPGenKey(
		IN  HCRYPTPROV hProv,
		IN  ALG_ID Algid,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey){
	try {
		CSPContext * it = *findContext(hProv);

		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &) {
		return FALSE;
	}
	return TRUE;
}

BOOL Csp::CPDeriveKey(
		IN  HCRYPTPROV hProv,
		IN  ALG_ID Algid,
		IN  HCRYPTHASH hHash,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey){
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &) {
		return FALSE;
	}
	return TRUE;
}

BOOL Csp::CPDestroyKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		it->m_keys.erase(it->findKeyContext(hKey));
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPSetKeyParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwParam,
		IN  CONST BYTE *pbData,
		IN  DWORD dwFlags){
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &) {
		return FALSE;
	}
	return TRUE;
}

BOOL Csp::CPGetKeyParam(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  DWORD dwParam,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		CSPKeyContext * key =  *it->findKeyContext(hKey);
		packData dat(pbData,pcbDataLen);
		flagCheck f(dwFlags,true);
		switch (dwParam) { //TODO: rest of the params
			case KP_ALGID:
				m_log << "KP_ALGID " << key->m_algId << std::endl;
				dat.setValue(key->m_algId);
				break;
			case KP_CERTIFICATE:
				m_log << "KP_CERTIFICATE " << key->m_algId << std::endl;
				if (GET_ALG_TYPE(key->m_algId) != ALG_TYPE_RSA) 
					throw err_badType();
				dat.setValue(key->m_certificateBlob);
				break;
			default:
				throw err_badType();
				break;
		}
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPExportKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  HCRYPTKEY hPubKey,
		IN  DWORD dwBlobType,
		IN  DWORD dwFlags,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		CSPKeyContext * key =  *it->findKeyContext(hKey);

		switch (dwBlobType) {
			case PUBLICKEYBLOB: {
				if (hPubKey) throw err_badKey();
				packData dat(pbData,pcbDataLen);
				struct {
					PUBLICKEYSTRUC  pubkeyStruc;
					RSAPUBKEY		rsaKey;
					BYTE			modulus[1024 / 8];
				} RSABlob;
				PCCERT_CONTEXT ctx = CertCreateCertificateContext(X509_ASN_ENCODING,
					&key->m_certificateBlob[0],(DWORD)key->m_certificateBlob.size());
				PCRYPT_BIT_BLOB pubKey = &ctx->pCertInfo->SubjectPublicKeyInfo.PublicKey;
				DWORD actSize = sizeof(RSABlob);
				err_crypto::check(CryptDecodeObject(X509_ASN_ENCODING,
					RSA_CSP_PUBLICKEYBLOB,pubKey->pbData,pubKey->cbData,
					0, &RSABlob, &actSize));
				dat.setValue(RSABlob);
				//key->m_certificateBlo
				ret.SetOk();
				break;
				}
			default:
				throw std::runtime_error("not implemented");
			}
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPImportKey(
		IN  HCRYPTPROV hProv,
		IN  CONST BYTE *pbData,
		IN  DWORD cbDataLen,
		IN  HCRYPTKEY hPubKey,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey){
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &) {
		return FALSE;
	}
	return TRUE;
}

BOOL Csp::CPDuplicateKey(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  LPDWORD pdwReserved,
		IN  DWORD dwFlags,
		OUT HCRYPTKEY *phKey){
	try {
		throw std::runtime_error("not implemented");
	} catch(std::runtime_error &) {
		return FALSE;
	}
	return TRUE;
}
