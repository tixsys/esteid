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

// EstEIDSigningBHO.h : Declaration of the CEstEIDSigningBHO

#pragma once
#include "resource.h"       // main symbols

#include "EstEIDSigningPluginBHO_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#include <shlguid.h>     // IID_IWebBrowser2, DIID_DWebBrowserEvents2, etc.
#include <mshtmhst.h> //IDochostUIHandler, IDocHostUIHandler2
#include <exdispid.h> // DISPID_DOCUMENTCOMPLETE, etc.
#include "SmartCardSigner.h"

// CEstEIDSigningBHO

class ATL_NO_VTABLE CEstEIDSigningBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEstEIDSigningBHO, &CLSID_EstEIDSigningBHO>,
	public IObjectWithSiteImpl<CEstEIDSigningBHO>,
	public IDocHostUIHandler2, //register as BHO
	public IDispatchImpl<IEstEIDSigningBHO, &IID_IEstEIDSigningBHO, &LIBID_EstEIDSigningPluginBHOLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventImpl<1, CEstEIDSigningBHO, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1> //Catch web browser events too
{
public:
	CEstEIDSigningBHO()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ESTEIDSIGNINGBHO)


BEGIN_COM_MAP(CEstEIDSigningBHO)
	COM_INTERFACE_ENTRY(IEstEIDSigningBHO)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDocHostUIHandler2)
END_COM_MAP()
BEGIN_SINK_MAP(CEstEIDSigningBHO)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
END_SINK_MAP()

// DWebBrowserEvents2
	void STDMETHODCALLTYPE OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL); 

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

private:
    CComPtr<IWebBrowser2>  m_iWebBrowser2; // WebBrowser host handle
	BOOL m_fAdvised; // 
	CComPtr<ISmartCardSigner> m_signer;

public:
	/// IDocHostUIHandler2 stubs
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	/*IDocHostUihandler*/
	STDMETHOD(GetExternal)(/* [out] */ IDispatch **ppDispatch);

	/// IDocHostUIHandler2 stubs
	HRESULT STDMETHODCALLTYPE ShowContextMenu( 
		/* [in] */ DWORD dwID,
		/* [in] */ POINT *ppt,
		/* [in] */ IUnknown *pcmdtReserved,
		/* [in] */ IDispatch *pdispReserved) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE GetHostInfo( 
		/* [out][in] */ DOCHOSTUIINFO *pInfo) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE ShowUI( 
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject *pActiveObject,
		/* [in] */ IOleCommandTarget *pCommandTarget,
		/* [in] */ IOleInPlaceFrame *pFrame,
		/* [in] */ IOleInPlaceUIWindow *pDoc) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE HideUI( void) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE UpdateUI( void) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE EnableModeless( 
		/* [in] */ BOOL fEnable) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
		/* [in] */ BOOL fActivate) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( 
		/* [in] */ BOOL fActivate) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE ResizeBorder( 
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow *pUIWindow,
		/* [in] */ BOOL fRameWindow) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID *pguidCmdGroup,
		/* [in] */ DWORD nCmdID) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
		/* [out] */ LPOLESTR *pchKey,
		/* [in] */ DWORD dw) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE GetDropTarget( 
		/* [in] */ IDropTarget *pDropTarget,
		/* [out] */ IDropTarget **ppDropTarget) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE TranslateUrl( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR *pchURLIn,
		/* [out] */ OLECHAR **ppchURLOut) {return S_FALSE;}
	HRESULT STDMETHODCALLTYPE FilterDataObject( 
		/* [in] */ IDataObject *pDO,
		/* [out] */ IDataObject **ppDORet) {return S_FALSE;}
   HRESULT STDMETHODCALLTYPE GetOverrideKeyPath( 
            /* [out] */ LPOLESTR *pchKey,
			/* [in] */ DWORD dw) { return FALSE;}
};

OBJECT_ENTRY_AUTO(__uuidof(EstEIDSigningBHO), CEstEIDSigningBHO)
