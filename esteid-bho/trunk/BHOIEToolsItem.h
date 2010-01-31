/*
* EstEIDSigningPluginBHO
*
* Copyright (C) 2009 Kaido Kert <kaidokert@gmail.com>
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
*
*/
// $Revision$
// $Date$
// $Author$

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
