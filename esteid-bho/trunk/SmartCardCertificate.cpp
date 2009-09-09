/*!
	\file		SmartCardCertificate.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-09-03 20:05:43 +0300 (Thu, 03 Sep 2009) $
*/
// Revision $Revision: 463 $

// SmartCardCertificate.cpp : Implementation of CSmartCardCertificate

#include "precompiled.h"
#include "SmartCardCertificate.h"
#include "utility/converters.h"
#include <algorithm>
#include <atlenc.h>

#define WINCRYPT 1
#pragma comment(lib,"crypt32")

// CSmartCardCertificate
STDMETHODIMP CSmartCardCertificate::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ISmartCardCertificate
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CSmartCardCertificate::get_CN(BSTR* pVal)
{
#if WINCRYPT
	PCCERT_CONTEXT cert = CertCreateCertificateContext(X509_ASN_ENCODING,&m_certBlob[0],(DWORD)m_certBlob.size());
	std::vector<WCHAR > chw(2000,0);
	DWORD strSz = (DWORD) chw.size();
	CertNameToStr(X509_ASN_ENCODING,&cert->pCertInfo->Subject,
		CERT_X500_NAME_STR,&chw[0],strSz);
	*pVal = _bstr_t(&chw[0]).Detach();
#endif
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_validFrom(BSTR* pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_validTo(BSTR* pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_issuerCN(BSTR* pVal)
{
#if WINCRYPT
	PCCERT_CONTEXT cert = CertCreateCertificateContext(X509_ASN_ENCODING,&m_certBlob[0],(DWORD)m_certBlob.size());
	std::vector<WCHAR > chw(2000,0);
	DWORD strSz = (DWORD) chw.size();
	CertNameToStr(X509_ASN_ENCODING,&cert->pCertInfo->Issuer ,
		CERT_X500_NAME_STR,&chw[0],strSz);
	*pVal = _bstr_t(&chw[0]).Detach();
#endif
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_keyUsage(BSTR* pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_cert(BSTR* pVal)
{
	int destLen = ATL::Base64EncodeGetRequiredLength(m_certBlob.size());
	std::vector<char> arrTarget(destLen+1,'0');
	ATL::Base64Encode(&m_certBlob[0], m_certBlob.size(), &arrTarget[0], &destLen);
	arrTarget.resize(destLen);
	arrTarget.push_back('\0');

	_bstr_t strCert = "-----BEGIN CERTIFICATE-----\n";
	strCert += &arrTarget[0];
	strCert += "\n-----END CERTIFICATE-----\n";
	*pVal = strCert.Detach();
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_certID(BSTR* pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

bool isSpaceChar(WCHAR value) {
	return value == L'\t' || value == L' ' 
		|| value == L'\n' || value == L'\r';
}

STDMETHODIMP CSmartCardCertificate::get_thumbPrint(BSTR* pVal)
{
	std::vector<unsigned char> arrTmp(0x20,0);
	DWORD sz = (DWORD) arrTmp.size(); 
#if WINCRYPT
	PCCERT_CONTEXT cert = CertCreateCertificateContext(X509_ASN_ENCODING,&m_certBlob[0],(DWORD)m_certBlob.size());
	CertGetCertificateContextProperty(
		cert,CERT_HASH_PROP_ID,&arrTmp[0],&sz);
	arrTmp.resize(sz);
	*pVal = _bstr_t(toHex(arrTmp).c_str()).Detach();
#endif
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_privateKeyContainer(BSTR* pVal)
{
	*pVal = _bstr_t(m_privateKeyContainer,true).Detach();
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::_loadArrayFrom(BSTR privateKeyContainer, ULONG length, BYTE* data)
{
	m_privateKeyContainer = privateKeyContainer;
	m_certBlob = std::vector<unsigned char>(data,data+length);
	return S_OK;
}
