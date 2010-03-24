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

// BHOIEToolsItem.cpp : Implementation of CBHOIEToolsItem

#include "precompiled.h"
#include "BHOIEToolsItem.h"
#include "comdef.h"

// CBHOIEToolsItem

STDMETHODIMP CBHOIEToolsItem::SetSite(IUnknown* pUnkSite) {
    if (pUnkSite != NULL) {
        // Cache the pointer to IWebBrowser2
        CComQIPtr<IServiceProvider> sp = pUnkSite;
		HRESULT hr = sp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&m_pBrowser);
		if (S_OK != hr) return hr;
		hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_ALL, IID_IInternetSecurityManager, (void **)&m_pInetSecMgr); 
		//hr = sp->QueryService(SID_SInternetSecurityManager, IID_IInternetSecurityManager, (void **)&m_pInetSecMgr);
	} else {
        // Release cached pointers
        m_pBrowser.Release();
		m_pInetSecMgr.Release();
		}

    return IObjectWithSiteImpl<CBHOIEToolsItem>::SetSite(pUnkSite);
}

STDMETHODIMP CBHOIEToolsItem::QueryStatus(const GUID *pguidCmdGroup, 
  ULONG cCmds, OLECMD *prgCmds, OLECMDTEXT *pCmdText) {
	if (pguidCmdGroup || (cCmds != 1) || !prgCmds) {
		return E_FAIL;
		}
	prgCmds->cmdf = OLECMDF_ENABLED;
	return S_OK;
}

STDMETHODIMP CBHOIEToolsItem::Exec(const GUID *pguidCmdGroup, 
  DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut) {
	HRESULT hResult = S_OK;
	BSTR bstrURL;
	//get current url
	hResult = m_pBrowser->get_LocationURL(&bstrURL);
	if (S_OK != hResult) return hResult;
	return m_pInetSecMgr->SetZoneMapping(URLZONE_TRUSTED, bstrURL, IsTrustedSite(bstrURL) ? SZM_DELETE : SZM_CREATE );
}

bool CBHOIEToolsItem::IsTrustedSite(BSTR& url)
{
	DWORD dwZone;
	HRESULT hResult = m_pInetSecMgr->MapUrlToZone(url, &dwZone, 0);
	if (S_OK == hResult) {
		if ((dwZone == URLZONE_LOCAL_MACHINE) ||
			(dwZone == URLZONE_INTRANET) ||
			(dwZone == URLZONE_TRUSTED)) {
			return true;
			}
		}
	return false;
}
