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
#include "RegKey.h"
#include "CSPTypes.h"
#include "utility.h"

#ifndef PP_USER_CERTSTORE //no Vista platformSDK
#define PP_USER_CERTSTORE 42
#define PP_ROOT_CERTSTORE 46
#define PP_SMARTCARD_GUID 45
#endif

Csp::Csp(HMODULE module,TCHAR *cspName) : 
	m_module(module),m_cspName(cspName),m_nextHandle(1),m_log("esteidcsp")
{
}

Csp::~Csp(void)
{
}

HRESULT Csp::DllRegisterServer(void) {
	try {
		RegKey providerKey(HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider"));
		RegKey cspKey(providerKey,m_cspName,KEY_WRITE);

		std::vector<TCHAR> modPath(MAX_PATH,'\0');
		DWORD sz = GetModuleFileNameW(m_module,&modPath[0],(DWORD)modPath.size());
		modPath.resize(sz+1);
		cspKey.setString(_T("Image Path"),&modPath[0]);
		cspKey.setInt(_T("Type"),getCSPType());
		cspKey.setInt(_T("SigInFile"),0);
	} catch(std::runtime_error &) {
		return S_FALSE;
		}
	return S_OK;
	}


HRESULT Csp::DllUnregisterServer(void) {
	try {
		RegKey providerKey(HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider"),KEY_WRITE);
		providerKey.deleteKey(m_cspName);
	} catch(std::runtime_error &) {
		return S_FALSE;
		}
	return S_OK;
	}


DWORD Csp::getNextHandle() {
	return ++m_nextHandle;
}

Csp::CSPContextIter Csp::findContext(HCRYPTPROV hProv) {
	ptrSearchTerm<HCRYPTPROV,CSPContext> term(hProv);
	CSPContextIter it = 
		find_if(m_contexts.begin(),m_contexts.end(),term);
	if (it == m_contexts.end()) throw err_badContext();
	return it;
	}

struct searchContainer {
	std::string &m_name,&m_reader;
	searchContainer(std::string &name,std::string &reader) : m_name(name),m_reader(reader) {}
};
bool operator==(CSPContext::cardLocation &loc,const searchContainer &ref) {
	std::string test = ref.m_name.length() ? ref.m_name : loc.containerName;
	if (ref.m_reader.length() && 
		ref.m_reader != loc.readerName ) return false;
	return test == loc.containerName;
	}

BOOL Csp::CPAcquireContext(
		OUT HCRYPTPROV *phProv,
		IN  LPCSTR szContainer,
		IN  DWORD dwFlags,
		IN  PVTableProvStruc pVTable){
	DECL_RET(ret);
	try {
		CSPContext* ctx = createCSPContext();
		flagCheck f(dwFlags);
		f.check(CRYPT_VERIFYCONTEXT,ctx->m_verifyContext);
		f.check(CRYPT_NEWKEYSET,ctx->m_newKeyset);
		f.check(CRYPT_DELETEKEYSET,ctx->m_deleteKeyset);
		f.check(CRYPT_MACHINE_KEYSET,ctx->m_machineKeyset);
		f.check(CRYPT_SILENT,ctx->m_silent);
		f.final();
		ctx->m_provId = getNextHandle();
		ctx->m_containerName  = szContainer == 0 ? "" : szContainer;
		m_log << "container : '" << ctx->m_containerName << "' flags:" << dwFlags << std::endl ;

		if (0 == ctx->m_containerName.compare(0,4,"\\\\.\\")) {
			std::string::size_type pos =  ctx->m_containerName.find('\\',5);
			ctx->m_readerName = ctx->m_containerName.substr(4,pos-4);
			ctx->m_containerName = ctx->m_containerName.substr(pos + 1);
			}
		searchContainer cont(ctx->m_containerName,ctx->m_readerName);
		std::vector<CSPContext::cardLocation>::iterator loc = 
			find(ctx->m_containers.begin(),ctx->m_containers.end(),cont);
		if (!ctx->m_verifyContext && loc == ctx->m_containers.end())
			throw err_badKeyset();
		if (loc != ctx->m_containers.end())
			ctx->m_containerName = loc->containerName;

		m_contexts.push_back(ctx);
		*phProv = ctx->m_provId;
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPAcquireContextW(
		OUT HCRYPTPROV *phProv,
		IN  LPCWSTR szContainer,
		IN  DWORD dwFlags,
		IN  PVTableProvStrucW pVTable){
	DECL_RET(ret);
	try {

		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPReleaseContext(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		m_contexts.erase(findContext(hProv));
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPSetProvParam(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwParam,
		IN  CONST BYTE *pbData,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		CSPContextIter it = findContext(hProv);
		switch(dwParam) {
			case PP_KEYEXCHANGE_PIN: {
				}
			case PP_SIGNATURE_PIN: {
				}
			}

		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPGetProvParam(
		IN  HCRYPTPROV hProv,
		IN  DWORD dwParam,
		OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD dwFlags){
	DECL_RET(ret);
	try {
		bool first;
		packData dat(pbData,pcbDataLen);
		CSPContext *it = *findContext(hProv);
		flagCheck f(dwFlags);
		f.check(CRYPT_FIRST,first);
		f.final();
		m_log << "dwParam: " << dwParam << " pcbDataLen:" << dat.m_originalSz << 
			" dwFlags:" << dwFlags << std::endl;
		switch (dwParam) {
			case PP_NAME :
				dat.setValue(m_cspName);break;
			case PP_VERSION:
				dat.setValue(DWORD(0x00000200));break;
			case PP_PROVTYPE:
				dat.setValue(DWORD(PROV_RSA_FULL));break;
			case PP_IMPTYPE :
				dat.setValue(DWORD(CRYPT_IMPL_MIXED));break;
			case PP_UNIQUE_CONTAINER :
			case PP_CONTAINER : {
				dat.setValue(it->m_containerName);
				break;
				}
			case PP_ENUMCONTAINERS:
				if (dwFlags & CRYPT_FIRST) {
					it->m_containersEnum.resize(it->m_containers.size());
					copy(it->m_containers.begin(),it->m_containers.end(),
						it->m_containersEnum.begin());
					}
				if (!it->m_containersEnum.empty()) {
					if (dwFlags & CRYPT_FIRST) m_log << "(CRYPT_FIRST) ";
					m_log << "container enum, return '" << it->m_containersEnum.back().containerName << "'" << std::endl;
					dat.setValue(it->m_containersEnum.back().containerName );
					it->m_containersEnum.pop_back();
					}
				else 
					throw err_noMoreItems();
				break;
			case PP_ENUMALGS :
				if (first) {
					BOOL success = TRUE;
					PROV_ENUMALGS alg;
					DWORD flag = CRYPT_FIRST;
					while(success){
						DWORD sz = sizeof(alg);
						success = CryptGetProvParam(
							*it->m_wrapCsp,PP_ENUMALGS,(LPBYTE)&alg,&sz,flag);
						if (success) m_enumAlgs.push_back(alg);
						flag = 0;
					}
					}
				else {
					if (m_enumAlgs.empty()) throw err_noMoreItems();
					dat.setValue(m_enumAlgs.back());
					m_enumAlgs.pop_back();
					}
				break;
			case PP_ENUMALGS_EX :
				if (first) {
					BOOL success = TRUE;
					PROV_ENUMALGS_EX  alg;
					DWORD flag = CRYPT_FIRST;
					while(success){
						DWORD sz = sizeof(alg);
						success = CryptGetProvParam(
							*it->m_wrapCsp,PP_ENUMALGS_EX,(LPBYTE)&alg,&sz,flag);
						if (success) m_enumAlgsEx.push_back(alg);
						flag = 0;
					}
					}
				else {
					if (m_enumAlgs.empty()) throw err_noMoreItems();
					dat.setValue(m_enumAlgs.back());
					m_enumAlgs.pop_back();
					}
				break;
			case PP_USER_CERTSTORE : {
					HCERTSTORE pStore = CertOpenStore(CERT_STORE_PROV_MEMORY,0,0,0,NULL);
					it->loadUserCerts();
					for(std::vector<CSPContext::cardLocation>::iterator i = it->m_containers.begin()
						; i != it->m_containers.end();i++) {
						if (!i->cert.size()) continue;
						PCCERT_CONTEXT ctx;
						CertAddEncodedCertificateToStore(pStore,X509_ASN_ENCODING,
							&(i->cert)[0], (DWORD)i->cert.size(), CERT_STORE_ADD_ALWAYS,&ctx);
						CRYPT_KEY_PROV_INFO propInfo;
						ZeroMemory(&propInfo,sizeof(propInfo));
						Widen<wchar_t> to_wstring;
						std::wstring wCont = to_wstring(i->containerName);
						propInfo.pwszContainerName = const_cast<LPWSTR>(wCont.c_str());
						propInfo.pwszProvName = (LPWSTR)m_cspName.c_str();
						propInfo.dwProvType = getCSPType();
						propInfo.dwFlags = 0;
						propInfo.cProvParam = 0;
						propInfo.rgProvParam = 0;
						propInfo.dwKeySpec = i->keySpec;
						CertSetCertificateContextProperty(ctx,CERT_KEY_PROV_INFO_PROP_ID,
							0,&propInfo);

						}
					dat.setValue(pStore);
					break;
				 }
			case PP_ROOT_CERTSTORE : {
					HCERTSTORE pStore = CertOpenStore(CERT_STORE_PROV_MEMORY,0,0,0,NULL);
					std::vector<std::vector<BYTE>> certs = it->getRootCerts();
					for(std::vector<std::vector<BYTE>>::iterator i = certs.begin()
						; i != certs.end();i++) {
						CertAddEncodedCertificateToStore(pStore,X509_ASN_ENCODING,
							&(*i)[0], (DWORD)i->size(), CERT_STORE_ADD_ALWAYS,NULL);
						}
					HCERTSTORE *pPstore = &pStore;
					dat.setValue(pPstore);
					break;
					}
			case PP_SMARTCARD_GUID : {
					const GUID bogus = { 0xe3daf770, 0x18e1, 0x4e29, { 0x99, 0x52, 0xe4, 0x5a, 0x7, 0x37, 0x5f, 0x84 } };
					dat.setValue(bogus);
					break;
					}
			case PP_SIG_KEYSIZE_INC :
			case PP_KEYX_KEYSIZE_INC:

			default:
				m_log << "requested: " << dwParam << " type" << std::endl;
				throw err_badType();
				break;
			}

		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPEncrypt(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  HCRYPTHASH hHash,
		IN  BOOL fFinal,
		IN  DWORD dwFlags,
		IN OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen,
		IN  DWORD cbBufLen){
	DECL_RET(ret);
	try {
		CSPContextIter it = findContext(hProv);
		ret.SetOk();
	} catch(std::runtime_error &err) {
		ret.logReturn(err);
	}
	return ret;
}

BOOL Csp::CPDecrypt(
		IN  HCRYPTPROV hProv,
		IN  HCRYPTKEY hKey,
		IN  HCRYPTHASH hHash,
		IN  BOOL fFinal,
		IN  DWORD dwFlags,
		IN OUT LPBYTE pbData,
		IN OUT LPDWORD pcbDataLen){
	DECL_RET(ret);
	try {
		CSPContext * it = *findContext(hProv);
		CSPKeyContext * key =  *it->findKeyContext(hKey);
		if (GET_ALG_TYPE(key->m_algId) != ALG_TYPE_RSA) 
			throw err_badType();
		packData dat(pbData,pcbDataLen);
		std::vector<BYTE> cipher(dat.m_pbData, dat.m_pbData + dat.m_originalSz);
		std::vector<BYTE> result = key->doRsaDecrypt( cipher);
		dat.setValue(result);
		ret.SetOk();
	} catch(std::runtime_error &) {
	}
	return ret;
}

BOOL Csp::CPGenRandom(
		IN  HCRYPTPROV hProv,
		IN  DWORD cbLen,
		OUT LPBYTE pbBuffer){
	DECL_RET(ret);
	try {
		CSPContext *it = *findContext(hProv);
		CryptGenRandom(*it->m_wrapCsp,cbLen,pbBuffer);
		ret.SetOk();
	} catch(std::runtime_error &) {
	}
	return ret;
}

