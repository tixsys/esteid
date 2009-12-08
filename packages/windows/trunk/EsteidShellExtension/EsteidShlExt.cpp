// EsteidShlExt.cpp : Implementation of CEsteidShlExt

#include "stdafx.h"
#include "EsteidShlExt.h"


CEsteidShlExt::CEsteidShlExt()
{
	m_DigidocBmp = LoadBitmap(_AtlBaseModule.GetModuleInstance(),
	                           MAKEINTRESOURCE(IDB_DIGIDOCBMP));
}

CEsteidShlExt::~CEsteidShlExt()
{
	if (m_DigidocBmp != NULL)
		DeleteObject(m_DigidocBmp);
}


STDMETHODIMP CEsteidShlExt::Initialize (
	LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID )
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;
	TCHAR szFile[MAX_PATH];
	HRESULT hr = S_OK;
	m_Files.clear();

	// Look for CF_HDROP data in the data object.
	if (FAILED(pDataObj->GetData(&fmt, &stg))) {
		// Nope! Return an "invalid argument" error back to Explorer.
		return E_INVALIDARG;
	}

	// Get a pointer to the actual data.
	hDrop = (HDROP) GlobalLock(stg.hGlobal);

	// Make sure it worked.
	if (hDrop == NULL) {
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	// Sanity check - make sure there is at least one filename.
	UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	if (nFiles == 0) {
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	for (UINT i = 0; i < nFiles; i++) {
		// Get path length in chars
		UINT len = DragQueryFile(hDrop, i, NULL, 0);
		if (len == 0 || len >= MAX_PATH)
			continue;

		// Get the name of the file
		if (DragQueryFile(hDrop, i, szFile, len+1) == 0)
			continue;

		tstring str = tstring(szFile);
		if (str.empty())
			continue;

		m_Files.push_back(str);
	}

	if (m_Files.empty()) {
		// Don't show menu if no items were found
		hr = E_INVALIDARG;
	}

	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);

	return hr;
}

STDMETHODIMP CEsteidShlExt::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags )
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	InsertMenu(hmenu, uMenuIndex, MF_STRING | MF_BYPOSITION, uidFirstCmd, _T("Allkirjasta ID-kaardiga"));
	if (m_DigidocBmp != NULL)
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_DigidocBmp, NULL);

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

STDMETHODIMP CEsteidShlExt::GetCommandString (
	UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax )
{
USES_CONVERSION;

	// Check idCmd, it must be 0 since we have only one menu item.
	if (idCmd != 0)
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if (uFlags & GCS_HELPTEXT) {
		LPCTSTR szText = _T("Allkirjasta valitud failid ID-kaardiga");

		if (uFlags & GCS_UNICODE) {
			// We need to cast pszName to a Unicode string, and then use the
			// Unicode string copy API.
			lstrcpynW((LPWSTR) pszName, T2CW(szText), cchMax);
		} else {
			// Use the ANSI string copy API to return the help string.
			lstrcpynA(pszName, T2CA(szText), cchMax);
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

STDMETHODIMP CEsteidShlExt::ExecuteDigidocclient(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	tstring command;
	tstring parameters;

	// Registry reading
	HKEY hkey;
	DWORD dwRet;
	DWORD dwSize = MAX_PATH * sizeof(TCHAR);
	TCHAR szInstalldir[MAX_PATH];

	dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, IDCARD_REGKEY, 0, KEY_QUERY_VALUE, &hkey);

	if (dwRet == ERROR_SUCCESS) {
		dwRet = RegQueryValueEx(hkey, IDCARD_REGVALUE, NULL, NULL, (LPBYTE)szInstalldir, &dwSize);
		RegCloseKey(hkey);
	} else {
		tstring title = _T("Error reading registry");
		tstring mess = _T("Failed to read registry value:\n");
		mess += _T("HKLM");
		mess += _T("\\");
		mess += IDCARD_REGKEY;
		mess += _T("\\");
		mess += IDCARD_REGVALUE;

		MessageBox(pCmdInfo->hwnd, mess.c_str(), title.c_str(),
		           MB_ICONERROR);

		return E_INVALIDARG;
	}


	command = tstring(szInstalldir);
	command += _T("qdigidocclient.exe");

	if (m_Files.empty())
		return E_INVALIDARG;

	// Construct command line arguments to pass to qdigidocclient.exe
	for (std::vector<tstring>::iterator it = m_Files.begin(); it != m_Files.end(); it++) {
		parameters += _T("\"");
		parameters += *(it);
		parameters += _T("\" ");
	}

	SHELLEXECUTEINFO  seInfo;
	memset(&seInfo, 0, sizeof(SHELLEXECUTEINFO));
	seInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
	seInfo.lpFile       = command.c_str();
	seInfo.lpParameters = parameters.c_str();
	seInfo.nShow        = SW_SHOW;
	ShellExecuteEx(&seInfo);

	return S_OK;
}

STDMETHODIMP CEsteidShlExt::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if (HIWORD(pCmdInfo->lpVerb) != 0)
		return E_INVALIDARG;

	// Get the command index - the only valid one is 0.
	switch (LOWORD(pCmdInfo->lpVerb)) {
	case 0:
		return ExecuteDigidocclient(pCmdInfo);
		break;

	default:
		return E_INVALIDARG;
		break;
	}
}
