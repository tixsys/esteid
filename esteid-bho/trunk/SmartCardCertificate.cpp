/*!
	\file		SmartCardCertificate.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 08:56:42 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 471 $

// SmartCardCertificate.cpp : Implementation of CSmartCardCertificate

#include "precompiled.h"
#include "SmartCardCertificate.h"
#include "utility/converters.h"
#include <algorithm>
#include <atlenc.h>
#include <strsafe.h>

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

CSmartCardCertificate::~CSmartCardCertificate()
{
	if (cert) 
		CertFreeCertificateContext(cert);
}


STDMETHODIMP CSmartCardCertificate::get_internal(BSTR* pVal,stringFields field) 
{
	SYSTEMTIME tm;
	CERT_EXTENSION* pExtn = NULL;
	std::vector<WCHAR > chw(2000,0);
	DWORD strSz = (DWORD) chw.size();
	std::ostringstream buf;

	switch(field) {
		case CN : 
			CertGetNameString(cert, CERT_NAME_ATTR_TYPE, 0,szOID_COMMON_NAME,&chw[0],strSz);break;
		case DN :
			CertNameToStr(X509_ASN_ENCODING,&cert->pCertInfo->Subject,
				CERT_X500_NAME_STR,&chw[0],strSz);break;
		case validFrom :
			FileTimeToSystemTime(&cert->pCertInfo->NotBefore,&tm);
			StringCchPrintf(&chw[0], strSz, TEXT("%02d.%02d.%04d %02d:%02d:%02d"),
					tm.wDay, tm.wMonth, tm.wYear,
					tm.wHour, tm.wMinute,tm.wSecond);
			break;
		case validTo :
			FileTimeToSystemTime(&cert->pCertInfo->NotAfter,&tm);
			StringCchPrintf(&chw[0], strSz, TEXT("%02d.%02d.%04d %02d:%02d:%02d"),
					tm.wDay, tm.wMonth, tm.wYear,
					tm.wHour, tm.wMinute,tm.wSecond);
			break;
		case issuerCN :
			CertGetNameString(cert, CERT_NAME_ATTR_TYPE, CERT_NAME_ISSUER_FLAG , szOID_COMMON_NAME,&chw[0],strSz);
			break;
		case issuerDN :
			CertNameToStr(X509_ASN_ENCODING,&cert->pCertInfo->Issuer ,
				CERT_X500_NAME_STR,&chw[0],strSz);break;
		case keyUsage :
				pExtn = CertFindExtension(
					szOID_KEY_USAGE,
					cert->pCertInfo->cExtension,
					cert->pCertInfo->rgExtension);
				if (pExtn)
					CryptFormatObject( X509_ASN_ENCODING, 0 ,0,0,szOID_KEY_USAGE, 
						pExtn->Value.pbData ,pExtn->Value.cbData,
						&chw[0],&strSz);
				break;
		case serial : {
			for(size_t i = cert->pCertInfo->SerialNumber.cbData ; i > 0  ;i--) 
				buf << std::hex << std::setfill('0') << std::setw(2) << (int) cert->pCertInfo->SerialNumber.pbData[i-1] << " ";
			std::string strBuf = buf.str();
			copy(strBuf.begin(),strBuf.end(),chw.begin());
			break;
			}
		default:
			return S_FALSE;
		}
	*pVal = _bstr_t(&chw[0]).Detach();
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_CN(BSTR* pVal)
{
	return get_internal(pVal,CN);
}

STDMETHODIMP CSmartCardCertificate::get_DN(BSTR* pVal)
{
	return get_internal(pVal,DN);
}

STDMETHODIMP CSmartCardCertificate::get_validFrom(BSTR* pVal)
{
	return get_internal(pVal,validFrom);
}

STDMETHODIMP CSmartCardCertificate::get_validTo(BSTR* pVal)
{
	return get_internal(pVal,validTo);
}

STDMETHODIMP CSmartCardCertificate::get_issuerCN(BSTR* pVal)
{
	return get_internal(pVal,issuerCN);
}

STDMETHODIMP CSmartCardCertificate::get_issuerDN(BSTR* pVal)
{
	return get_internal(pVal,issuerDN);
}

STDMETHODIMP CSmartCardCertificate::get_keyUsage(BSTR* pVal)
{
	return get_internal(pVal,keyUsage);
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
	CertGetCertificateContextProperty(
		cert,CERT_HASH_PROP_ID,&arrTmp[0],&sz);
	arrTmp.resize(sz);
	*pVal = _bstr_t(toHex(arrTmp).c_str()).Detach();
	return S_OK;
}

STDMETHODIMP CSmartCardCertificate::get_serial(BSTR* pVal)
{
	return get_internal(pVal,serial);
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
	cert = CertCreateCertificateContext(X509_ASN_ENCODING,&m_certBlob[0],(DWORD)m_certBlob.size());
	return S_OK;
}
