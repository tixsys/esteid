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

// EstEIDSigningBHO.cpp : Implementation of CEstEIDSigningBHO

#include "precompiled.h"
#include "EstEIDSigningBHO.h"


// CEstEIDSigningBHO


HRESULT CEstEIDSigningBHO::FinalConstruct()
{
//	m_signer.CoCreateInstance(CLSID_SmartCardSigner);
	return S_OK;
}

void CEstEIDSigningBHO::FinalRelease()
{
}


// Csinglesignon
STDMETHODIMP CEstEIDSigningBHO::SetSite(IUnknown* pUnkSite) {
    if (pUnkSite != NULL) { // Cache the pointer to IWebBrowser2.
        HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void**)&m_iWebBrowser2);
        if (SUCCEEDED(hr)) { // Register to sink events from DWebBrowserEvents2.
            hr = DispEventAdvise(m_iWebBrowser2);
            if (SUCCEEDED(hr))
				m_fAdvised = TRUE;
	        }
	} else { // Unregister event sink.
        if (m_fAdvised) {
            DispEventUnadvise(m_iWebBrowser2);
            m_fAdvised = FALSE;
	        }
        m_iWebBrowser2.Release();
	    }
    return IObjectWithSiteImpl<CEstEIDSigningBHO>::SetSite(pUnkSite);
}
STDMETHODIMP CEstEIDSigningBHO::GetExternal(  //return a signer object
    /* [out] */ IDispatch **ppDispatch) {
/*	CComPtr<IDispatch> in;
	in = m_signer;
	*ppDispatch = in.Detach();
*/
	return S_FALSE;
};

void STDMETHODCALLTYPE CEstEIDSigningBHO::OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL) {
	CComPtr<IDispatch> spDispDoc;
	if (FAILED(m_iWebBrowser2->get_Document(&spDispDoc))) return;

	// ...and query for an HTML document.
#if 0 //disabled , used as ActiveX
	CComQIPtr<IHTMLDocument2> pMainWinDocument = spDispDoc;
    if (pMainWinDocument == NULL) return;

	CComQIPtr<ICustomDoc > pCustomDoc;
	if (SUCCEEDED(pMainWinDocument->QueryInterface(IID_ICustomDoc, (void**)&pCustomDoc)))
		if (pCustomDoc) 
			pCustomDoc->SetUIHandler(this);			
#endif
}
