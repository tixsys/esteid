/*!
	\file		CspTypes.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-06 22:15:42 +0300 (Mon, 06 Apr 2009) $
*/
// Revision $Revision: 228 $

#include "precompiled.h"
#include "CSPTypes.h"
#include "CSPErrors.h"
#include <algorithm>

CSPContext::CSPContext(logger &log): m_log(log) {
	m_wrapCsp =  new WrapCsp(MS_ENHANCED_PROV,PROV_RSA_FULL);
	}

CSPContext::CSPContext(logger &log,const CSPContext &c) : m_log(log) {
	m_provId=c.m_provId;
	m_provId=c.m_provId;
	m_flags=c.m_flags;
	m_verifyContext=c.m_verifyContext;
	m_newKeyset=c.m_newKeyset;
	m_machineKeyset=c.m_machineKeyset;
	m_deleteKeyset=c.m_deleteKeyset;
	m_silent=c.m_silent;
	m_wrapCsp = new WrapCsp(MS_ENHANCED_PROV,PROV_RSA_FULL);
	}

CSPContext::~CSPContext() {
	delete m_wrapCsp;
}

CSPHashContext * CSPContext::createHashContext() {
	return new CSPHashContext();
	}

CSPKeyContext * CSPContext::createKeyContext() {
	return new CSPKeyContext();
	}

bool CSPContext::operator==(const HCRYPTPROV prov) const {
	return m_provId == prov;
	}

CSPHashContextIter CSPContext::findHashContext(HCRYPTHASH hProv) { 
	ptrSearchTerm<HCRYPTHASH,CSPHashContext> term(hProv);
	CSPHashContextIter it = 
		find_if(m_hashes.begin(),m_hashes.end(),term);
	if (it == m_hashes.end()) throw err_badHash();
	return it;	
}
CSPKeyContextIter CSPContext::findKeyContext(HCRYPTKEY hKey) {
	ptrSearchTerm<HCRYPTKEY,CSPKeyContext> term(hKey);
	CSPKeyContextIter it = 
		find_if(m_keys.begin(),m_keys.end(), term);
	if (it == m_keys.end()) throw err_badKey();
	return it;
}

bool CSPKeyContext::operator==(const HCRYPTKEY keyId) const {
	return m_keyId == keyId;
}

bool CSPHashContext::operator==(const HCRYPTHASH hashId) const {
	return m_hashId == hashId;
}

