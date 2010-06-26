/*!
	\file		threadObj.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-06-17 23:57:20 +0300 (Wed, 17 Jun 2009) $
*/
// Revision $Revision: 323 $

#include "precompiled.h"
#include "threadObj.h"
#include <iostream>
#ifndef _WIN32 //pthreads
#include <pthread.h>
struct mutexPriv {
	pthread_mutex_t m_mutex;
	mutexPriv() {
        pthread_mutex_init(&m_mutex,NULL);
        }
    ~mutexPriv() {
        pthread_mutex_destroy(&m_mutex);
        }
    bool Lock() {
       return  !pthread_mutex_lock(&m_mutex);
       }
    bool Unlock() {
        return pthread_mutex_unlock(&m_mutex);
        }
};
struct threadPriv {
    pthread_t m_id;
    threadObj &m_threadObj;
    static void * thread_entry(void * const thisPtr) {
        threadPriv * const this_ = (threadPriv*) thisPtr;
        this_->m_threadObj.execute();
        return 0;
         }
    threadPriv(threadObj &ref) : m_threadObj(ref) {
        }
    void start() {
        pthread_create(&m_id,NULL,thread_entry,this);
       }
    void stop() {
        pthread_cancel(m_id);
        }
    ~threadPriv() {
        stop();
        }
    static void wait(unsigned int ms) {
        timespec timesp;
        timesp.tv_sec =(time_t) (ms/ 1000);
        timesp.tv_nsec=(time_t) ((ms% 1000) * 1000 * 1000);
        nanosleep(&timesp,NULL);
        }
};
#else
#include <windows.h>
struct threadPriv {
    DWORD m_id;
    HANDLE m_handle;
    threadObj &m_threadObj;
    static DWORD WINAPI thread_entry( LPVOID thisPtr) {
        threadPriv * const this_ = (threadPriv*) thisPtr;
        this_->m_threadObj.execute();
		return 0;
        }
    threadPriv(threadObj &ref) : m_threadObj(ref) {
        }
    void start() {
        m_handle = CreateThread(NULL,0,thread_entry,this,0,&m_id);
        }
    void stop() {
        TerminateThread(m_handle,0);
        }
    ~threadPriv() {
        stop();
        }
    static void wait(unsigned int ms) {
        Sleep(ms);
        }
private:
	const threadPriv &operator=(const threadPriv &o);
};
struct mutexPriv {
    CRITICAL_SECTION cs;
	mutexPriv() {
	    InitializeCriticalSection(&cs);
        }
    ~mutexPriv() {
        DeleteCriticalSection(&cs);
        }
    bool Lock() {
        EnterCriticalSection(&cs);
        return true;
       }
    bool Unlock() {
        LeaveCriticalSection(&cs);
		return true;
		}
private:
	const mutexPriv &operator=(const mutexPriv &o);
};
#endif

mutexObj::mutexObj(const char *name) : m_name(name) , d(new mutexPriv){}
mutexObj::~mutexObj() {delete d;}
bool mutexObj::Lock() {return d->Lock();}
bool mutexObj::Unlock() {return d->Unlock();}

threadObj::threadObj(const char *name) : m_name(name) {
	d = new threadPriv(*this);
	}
threadObj::~threadObj() {delete d;}
void threadObj::wait(unsigned int ms) {threadPriv::wait(ms);}
void threadObj::start() {d->start();}
void threadObj::stop() {d->stop();}

