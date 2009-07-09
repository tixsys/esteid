#include "nsEstEIDService.h"

#include <nsStringAPI.h>
#include <nsISupportsImpl.h>
#include <nsWeakReference.h>
#include <nsComponentManagerUtils.h>
#include <nsServiceManagerUtils.h>

#include <iostream>

NS_IMPL_ISUPPORTS1( nsEstEIDService, nsITimerCallback );

NS_IMETHODIMP nsEstEIDService::Notify(nsITimer *timer) {
	sEstEIDService->Worker();
	return NS_OK;
}

void nsEstEIDService::PostMessage(msgType type,
		readerID reader, std::string msg) {

	nsString types[] = {
			NS_LITERAL_STRING("cardInserted"),
			NS_LITERAL_STRING("cardRemoved"),
			NS_LITERAL_STRING("readersChanged"),
			NS_LITERAL_STRING("cardError")
	};

	if(type > 2) {  // Unknown message type
		std::cout << "Unknown message type:" << type << std::endl;
		return;
	}

	nsresult rv = anObserverService->NotifyObservers(nsnull,
			NS_LITERAL_CSTRING("esteid-card-changed").get(),
			types[type].get());

	if(NS_FAILED(rv)) {
		// FIXME: Log error
	}
}

nsEstEIDService::nsEstEIDService() {
	std::cout << "nsEstEIDService()" << std::endl;
	nsresult rv;

	lock = PR_NewLock();
    if(!lock)
    	throw std::runtime_error("Out of memory");
    anObserverService =
    		do_GetService(NS_OBSERVERSERVICE_CONTRACTID);

    if(!anObserverService)
    	throw std::runtime_error("Can't access"NS_OBSERVERSERVICE_CONTRACTID);

	mTimer = do_CreateInstance(NS_TIMER_CONTRACTID);
	if(!mTimer)
		throw std::runtime_error("Out of memory");

    rv = mTimer->InitWithCallback(this, 500,
	                              nsITimer::TYPE_REPEATING_SLACK);
    if(NS_FAILED(rv))
    	throw std::runtime_error("Can't create timer");
}

nsEstEIDService::~nsEstEIDService() {
	PR_DestroyLock(lock);
	std::cout << "~nsEstEIDService()" << std::endl;
	// FIXME: Maybe kill timer?
};
