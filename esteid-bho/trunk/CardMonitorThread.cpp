/*!
	\file		CardMonitorThread.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-03-29 17:48:22 +0300 (Sun, 29 Mar 2009) $
*/
// Revision $Revision: 205 $

#include "precompiled.h"
#include "CardMonitorThread.h"
#include "cardlib/SmartCardManager.h"
#include "cardlib/EstEidCard.h"
#include <vector>

struct WorkerPriv {
	SmartCardManager mgr;
	std::vector<bool> cardPresent;
	CWindow *win;
	EstEidCard card;
	WorkerPriv() : card(mgr) {}
	bool executeCheck();
	void SafePostMessage(UINT msg,WPARAM wParam) {
		if (win->IsWindow()) win->PostMessage(msg,wParam);
		}
	bool readerHasCard(EstEidCard &card,int i) {
		std::string state = mgr.getReaderState(i);
		if (state.find("PRESENT") == std::string::npos ) return false;
		if (cardPresent[i]) //caching/optimization
			return true;
		return card.isInReader(i);
		}
};

bool WorkerPriv::executeCheck() {
	int readers = mgr.getReaderCount(true);
	if (readers != cardPresent.size()) {
		cardPresent.resize(readers);
		SafePostMessage(WM_READERS_CHANGED,readers);
		}
	for (size_t i = 0; i < cardPresent.size() ; i++ ) {
		if (readerHasCard(card,i) && !cardPresent[i]) {
			ATLTRACE("card inserted\n");
			cardPresent[i] = true;
			SafePostMessage(WM_CARD_INSERTED,i);
			}
		if (!readerHasCard(card,i) && cardPresent[i]) {
			ATLTRACE("card removed\n");
			cardPresent[i] = false;
			SafePostMessage(WM_CARD_REMOVED,i);
			}
		}
	return true;
	}

CardMonitorThread::CardMonitorThread(CWindow &parent,
	 CComCriticalSection &criticalSection) : 
		d(new WorkerPriv),m_criticalSection(criticalSection)
{
	d->win = &parent;
	m_thread.Initialize();
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_thread.AddHandle(m_hEvent, this, NULL);
	::SetEvent(m_hEvent);
}

CardMonitorThread::~CardMonitorThread(void)
{
	m_thread.RemoveHandle(m_hEvent);
	m_thread.Shutdown();
	delete d;
	d = NULL;
}

HRESULT CardMonitorThread::Execute(DWORD_PTR dwParam, HANDLE hObject) {
	Sleep(500);
	m_criticalSection.Lock();
	try {
		d->executeCheck();
	} catch(std::runtime_error &err) {
		int k = 1;
		}
	m_criticalSection.Unlock();
	return S_OK;
	}

HRESULT CardMonitorThread::CloseHandle(HANDLE hHandle) {
	::CloseHandle(hHandle);
	return S_OK;
}
