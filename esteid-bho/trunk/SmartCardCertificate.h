/*
* EstEIDSigningPluginBHO
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

// SmartCardCertificate.h : Declaration of the CSmartCardCertificate

#pragma once
#include "resource.h"       // main symbols

#include "EstEIDSigningPluginBHO_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSmartCardCertificate

class ATL_NO_VTABLE CSmartCardCertificate :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSmartCardCertificate, &CLSID_SmartCardCertificate>,
	public ISupportErrorInfo,
	public IObjectSafetyImpl<CSmartCardCertificate, INTERFACESAFE_FOR_UNTRUSTED_CALLER |
         INTERFACESAFE_FOR_UNTRUSTED_DATA >,
	public IProvideClassInfo2Impl<&CLSID_SmartCardCertificate,&IID_ISmartCardCertificate>,
	public IDispatchImpl<ISmartCardCertificate, &IID_ISmartCardCertificate, &LIBID_EstEIDSigningPluginBHOLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CSmartCardCertificate() : cert(NULL)
	{
	}
	virtual ~CSmartCardCertificate(); 

DECLARE_REGISTRY_RESOURCEID(IDR_SMARTCARDCERTIFICATE)


BEGIN_COM_MAP(CSmartCardCertificate)
	COM_INTERFACE_ENTRY(ISmartCardCertificate)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHOD(get_CN)(BSTR* pVal);
	STDMETHOD(get_DN)(BSTR* pVal);
	STDMETHOD(get_validFrom)(BSTR* pVal);
	STDMETHOD(get_validTo)(BSTR* pVal);
	STDMETHOD(get_issuerCN)(BSTR* pVal);
	STDMETHOD(get_issuerDN)(BSTR* pVal);
	STDMETHOD(get_keyUsage)(BSTR* pVal);
	STDMETHOD(get_cert)(BSTR* pVal);
	STDMETHOD(get_certID)(BSTR* pVal);
	STDMETHOD(get_thumbPrint)(BSTR* pVal);
	STDMETHOD(get_serial)(BSTR* pVal);
	STDMETHOD(get_privateKeyContainer)(BSTR* pVal);
	STDMETHOD(_loadArrayFrom)(BSTR privateKeyContainer, ULONG length, BYTE* data);
private:
	enum stringFields {
		CN,
		DN,
		validFrom,
		validTo,
		issuerCN,
		issuerDN,
		keyUsage,
		serial
	};

	/// hold the actual cert data
	std::vector<unsigned char> m_certBlob;
	/// keep the cert context
	PCCERT_CONTEXT cert;
	/// where the key is <CSP:NAME:CONTAINER:KEYID|CARD:NAME:ID|PKCS11MOD:SLOT:xx> 
	_bstr_t m_privateKeyContainer;
	///
	STDMETHOD(get_internal)(BSTR* pVal,stringFields field);
};

OBJECT_ENTRY_AUTO(__uuidof(SmartCardCertificate), CSmartCardCertificate)
