/*
 * esteid-csp - CSP for Estonian EID card
 *
 * Copyright (C) 2008-2010  Estonian Informatics Centre
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
