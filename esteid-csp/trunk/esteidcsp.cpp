/*!
	\file		esteidcsp.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-08 02:26:37 +0300 (Wed, 08 Apr 2009) $
*/
// Revision $Revision: 233 $

#include "precompiled.h"
#include "Setup.h"
#include "CSPEstEid.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif
#if defined(_DEBUG) && defined(AVOID_SERVICE_LOAD)
bool dll_loadedFromService() {
	HMODULE caller = GetModuleHandle(NULL);
	std::vector<WCHAR > callerExe(MAX_PATH + 1,'\0');
	GetModuleFileNameW(caller,&callerExe[0],MAX_PATH);
	std::vector<WCHAR >::iterator fl = --callerExe.end();
	while (isalnum(*fl) || (L'.' == *fl) || (L'_' == *fl) || L'\0' == *fl) fl--;
	fl++;
	std::wstring exeName(&*fl, &*fl + lstrlenW(&*fl) );
	if (exeName == L"explorer.exe") return true;
	if (exeName == L"lsass.exe") return true;
	if (exeName == L"winlogon.exe") return true;
	if (exeName == L"svchost.exe") return true;
	//if (exeName == L"loadTest.exe") return true;
	return false;
	}
#else
bool dll_loadedFromService() { return false ; }
#endif

Csp *csp = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (dll_loadedFromService()) return FALSE;
		csp = new CspEstEid(hModule,TEXT("EstEID NewCard CSP"));
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		delete csp;
		csp = NULL;
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
