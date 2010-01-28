/*!
	\file		BHOIEToolsItem.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-21 08:43:53 +0300 (K, 21 okt 2009) $
*/
// Revision $Revision: 0 $

// BHOIEToolsItem.h : Declaration of the CBHOIEToolsItem

#pragma once
#include "resource.h"       // main symbols
#include "EstEIDSigningPluginBHO_i.h"

#include <shlguid.h>
#include <docobj.h> //IOleCommandTarget
#include <mshtml.h>
#include <urlmon.h>
#include <shlobj.h>
#include <objsafe.h> //IObjectSafetyImpl

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CBHOIEToolsItem
class ATL_NO_VTABLE CBHOIEToolsItem :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBHOIEToolsItem, &CLSID_BHOIEToolsItem>,
	public IOleCommandTarget,
	public IObjectSafetyImpl<CBHOIEToolsItem, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA >,
	public IObjectWithSiteImpl<CBHOIEToolsItem>,
	public IDispatchImpl<IBHOIEToolsItem, &IID_IBHOIEToolsItem, &LIBID_EstEIDSigningPluginBHOLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CBHOIEToolsItem() {
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BHOIETOOLSITEM)

DECLARE_NOT_AGGREGATABLE(CBHOIEToolsItem)

BEGIN_COM_MAP(CBHOIEToolsItem)
	COM_INTERFACE_ENTRY(IBHOIEToolsItem)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
    CComPtr<IWebBrowser2> m_pBrowser;
	CComPtr<IInternetSecurityManager> m_pInetSecMgr;

private:
	bool IsTrustedSite(BSTR& url); 

public:
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD *prgCmds, OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut );
};

OBJECT_ENTRY_AUTO(__uuidof(BHOIEToolsItem), CBHOIEToolsItem)
