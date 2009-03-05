#ifndef nsEstEIDService_h
#define nsEstEIDService_h

/* Mozilla includes */
#include <prthread.h>
#include <prlock.h>
#include <nsIObserverService.h>
#include <nsCOMPtr.h>
#include <nsITimer.h>

#include "EstEIDService.h"

class nsEstEIDService : public EstEIDService <nsEstEIDService>,
                        public nsITimerCallback {
public:
	NS_DECL_ISUPPORTS
	NS_DECL_NSITIMERCALLBACK

protected:
	void Lock()   { PR_Lock(lock);   }
	void UnLock() { PR_Unlock(lock); }
	void PostMessage(msgType, readerID, std::string);
private:
	friend class EstEIDService<nsEstEIDService>;
    nsEstEIDService();
    ~nsEstEIDService();
    PRLock *lock;
    nsCOMPtr<nsIObserverService> anObserverService;
    nsCOMPtr<nsITimer> mTimer;
};
#endif // nsEstEIDService_h
