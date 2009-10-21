// Sign.h : Declaration of the CSign

#ifndef __SIGN_H_
#define __SIGN_H_

#include "resource.h"       // main symbols
#include <atlctl.h>
//versiooni tekst
#define INFOTEXT "EIDCard Sign Module 1.0.2.9 version 20-October-2009"
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
#define KEYPATH "SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider\\"
#define MAX_LEN 1000


/////////////////////////////////////////////////////////////////////////////
// CSign
class ATL_NO_VTABLE CSign : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSign, &CLSID_Sign>,
	public ISupportErrorInfo,
	public IDispatchImpl<ISign, &IID_ISign, &LIBID_EIDCARDLib>,
	public IObjectSafetyImpl<CSign, INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{
public:
	CSign()
	{

	}

DECLARE_REGISTRY_RESOURCEID(IDR_SIGN)
DECLARE_NOT_AGGREGATABLE(CSign)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSign)
	COM_INTERFACE_ENTRY(ISign)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ISign
public:
	STDMETHOD(get_selectedCertNumber)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_selectedCertNumber)(/*[in]*/ long newVal);

	STDMETHOD(getSignedHash)(/*[in]*/ BSTR sHash,/*[in]*/ long selectedCert, /*[out, retval]*/ BSTR* SignedHash);
	STDMETHOD(getSigningCertificate)(/*[out, retval]*/ BSTR* Certificate);
	STDMETHOD(getInfo)(/*[out, retval]*/ BSTR* infoText);
    
	DWORD gdwSelectedCertNumber;

};

#endif //__SIGN_H_
