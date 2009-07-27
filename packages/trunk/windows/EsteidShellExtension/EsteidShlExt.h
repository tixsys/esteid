// EsteidShlExt.h : Declaration of the CEsteidShlExt

#pragma once
#include "resource.h"       // main symbols

#include "EsteidShellExtension_i.h"

#include <shlobj.h>
#include <comdef.h>
#include <winreg.h>


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define IDCARD_REGKEY L"SOFTWARE\\Estonian ID Card"
#define IDCARD_REGVALUE L"Installed"



// CEsteidShlExt

class ATL_NO_VTABLE CEsteidShlExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEsteidShlExt, &CLSID_EsteidShlExt>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CEsteidShlExt();
	~CEsteidShlExt();

	DECLARE_REGISTRY_RESOURCEID(IDR_ESTEIDSHLEXT)
	DECLARE_NOT_AGGREGATABLE(CEsteidShlExt)

	BEGIN_COM_MAP(CEsteidShlExt)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

protected:
	HBITMAP  m_DigidocBmp;
	TCHAR    m_szFile[MAX_PATH];

	STDMETHODIMP CEsteidShlExt::ExecuteDigidocclient(LPCMINVOKECOMMANDINFO pCmdInfo);

public:
	// IShellExtInit
	STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
	STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);
};

OBJECT_ENTRY_AUTO(__uuidof(EsteidShlExt), CEsteidShlExt)
