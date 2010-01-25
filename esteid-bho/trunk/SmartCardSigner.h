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
// $Revision: 502 $
// $Date: 2010-01-25 03:07:01 +0200 (E, 25 jaan 2010) $
// $Author: kaidokert $

// SmartCardSigner.h : Declaration of the CSmartCardSigner

#pragma once
#include "resource.h"       // main symbols

#include "EstEIDSigningPluginBHO_i.h"
#include "_ISmartCardSignerEvents_CP.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#include <atlwin.h> //CWindowImpl
#include <objsafe.h> //IObjectSafetyImpl
#include <atlutil.h> //IWorkerThreadClient,CInterfaceList
#include <smartcard++/common.h>
#include <smartcard++/esteid/EstEidCard.h>
#include <smartcard++/SmartCardManager.h>
#include <utility/monitorThread.h>
#include <utility/logger.h>
#include "EstEIDSigningPluginBHO_i.h"

#define WM_CARD_INSERTED   (WM_USER + 101)
#define WM_CARD_REMOVED    (WM_USER + 102)
#define WM_READERS_CHANGED (WM_USER + 103)

// CSmartCardSigner

class ATL_NO_VTABLE CSmartCardSigner :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSmartCardSigner, &CLSID_SmartCardSigner>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CSmartCardSigner>,
	public CProxy_ISmartCardSignerEvents<CSmartCardSigner>,
	public IObjectWithSiteImpl<CSmartCardSigner>,
	public IObjectSafetyImpl<CSmartCardSigner, INTERFACESAFE_FOR_UNTRUSTED_CALLER |
	INTERFACESAFE_FOR_UNTRUSTED_DATA >,
	public IProvideClassInfo2Impl<&CLSID_SmartCardSigner,&IID_ISmartCardSigner>,
	public CWindowImpl<CSmartCardSigner>, //window to receive 2nd thread notifications
	public IDispatchImpl<ISmartCardSigner, &IID_ISmartCardSigner, &LIBID_EstEIDSigningPluginBHOLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public monitorObserver
{
public:
	CSmartCardSigner();

	DECLARE_REGISTRY_RESOURCEID(IDR_SMARTCARDSIGNER)


	BEGIN_COM_MAP(CSmartCardSigner)
		COM_INTERFACE_ENTRY(ISmartCardSigner)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IObjectSafety)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CSmartCardSigner)
		CONNECTION_POINT_ENTRY(__uuidof(_ISmartCardSignerEvents))
	END_CONNECTION_POINT_MAP()

	BEGIN_MSG_MAP(CSmartCardSigner)
		MESSAGE_HANDLER(WM_READERS_CHANGED, OnReadersChanged)
		MESSAGE_HANDLER(WM_CARD_INSERTED, OnCardInserted)
		MESSAGE_HANDLER(WM_CARD_REMOVED, OnCardRemoved)
	END_MSG_MAP()


	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	LRESULT OnReadersChanged(UINT uMsg, WPARAM wParam,
		LPARAM lParam, BOOL& bHandled);
	LRESULT OnCardInserted(UINT uMsg, WPARAM wParam,
		LPARAM lParam, BOOL& bHandled);
	LRESULT OnCardRemoved(UINT uMsg, WPARAM wParam,
		LPARAM lParam, BOOL& bHandled);
	
	void onEvent(monitorEvent eType,int param);
	HRESULT FinalConstruct();
	void FinalRelease();
	STDMETHOD(SetSite)(IUnknown *pUnkSite);

private:
	logger m_log;
	bool runningInSecureZone;
    CComPtr<IWebBrowser2>  m_iWebBrowser2; // WebBrowser host handle

	STDMETHOD(errMsg)(LPCOLESTR err);
	STDMETHOD(errMsg)(std::string err);
	STDMETHOD(readField)(EstEidCard::RecordNames rec, BSTR *pVal);

//	CardMonitorThread *m_monitorThread;
	SmartCardManager m_mgr;
	uint m_selectedReader;
	std::vector<std::string> m_cardData;
	std::vector<unsigned char> m_authCert;
	std::vector<unsigned char> m_signCert;
	void clearCaches();
	void getEstEIDCerts(CInterfaceList<ISmartCardCertificate> &list);
	CInterfaceList<IDispatch> notifyCardInsert;
	CInterfaceList<IDispatch> notifyCardRemove;
	CInterfaceList<IDispatch> notifyReadersChanged;
	mutexObj criticalSection; //avoid same-time access from both threads
	monitorThread *m_monitorThread;
public:
	STDMETHOD(get_lastName)(BSTR* pVal);
	STDMETHOD(get_firstName)(BSTR* pVal);
	STDMETHOD(get_middleName)(BSTR* pVal);
	STDMETHOD(get_sex)(BSTR* pVal);
	STDMETHOD(get_citizenship)(BSTR* pVal);
	STDMETHOD(get_birthDate)(BSTR* pVal);
	STDMETHOD(get_personalID)(BSTR* pVal);
	STDMETHOD(get_documentID)(BSTR* pVal);
	STDMETHOD(get_expiryDate)(BSTR* pVal);
	STDMETHOD(get_placeOfBirth)(BSTR* pVal);
	STDMETHOD(get_issuedDate)(BSTR* pVal);
	STDMETHOD(get_residencePermit)(BSTR* pVal);
	STDMETHOD(get_comment1)(BSTR* pVal);
	STDMETHOD(get_comment2)(BSTR* pVal);
	STDMETHOD(get_comment3)(BSTR* pVal);
	STDMETHOD(get_comment4)(BSTR* pVal);
	STDMETHOD(signWithCert)(BSTR hashToBeSigned,IDispatch *pCert,BSTR* pVal);
	STDMETHOD(getCertificateList)(BSTR* retVal);
	STDMETHOD(getCertificateByThumbprint)(BSTR thumbPrint, IDispatch** retVal);
	STDMETHOD(addEventListener)(BSTR eventName,IDispatch *unk);
	STDMETHOD(removeEventListener)(BSTR eventName,IDispatch *unk);
	STDMETHOD(getVersion)(BSTR* retVal);
	STDMETHOD(getReaderName)(USHORT readerNum,BSTR* retVal);
	STDMETHOD(getReaders)(BSTR* retVal);
	STDMETHOD(get_selectedReader)(SHORT* pVal);
	STDMETHOD(put_selectedReader)(SHORT newVal);
	STDMETHOD(get_authCert)(IDispatch** pVal);
	STDMETHOD(get_signCert)(IDispatch** pVal);
	STDMETHOD(sign)(BSTR hashToBeSigned, BSTR documentUrl, BSTR* pVal);
	};

OBJECT_ENTRY_AUTO(__uuidof(SmartCardSigner), CSmartCardSigner)
