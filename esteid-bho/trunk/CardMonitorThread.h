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

#pragma once

#include <atlutil.h> //IWorkerThreadClient

struct WorkerPriv;

#define WM_CARD_INSERTED   (WM_USER + 101)
#define WM_CARD_REMOVED    (WM_USER + 102)
#define WM_READERS_CHANGED (WM_USER + 103)

class CardMonitorThread :
	public IWorkerThreadClient
{
	HANDLE m_hEvent;
	CWorkerThread<Win32ThreadTraits> m_thread;
	CComCriticalSection &m_criticalSection;
	WorkerPriv* d;
public:
	CardMonitorThread(CWindow &parent,CComCriticalSection &criticalSection);
	virtual ~CardMonitorThread(void);
private:
    HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
	HRESULT CloseHandle(HANDLE hHandle);
};
