#ifndef nsEstEID_h
#define nsEstEID_h

#include "cardlib.h"

#include "nsIEstEID.h"
#include "nsEstEIDService.h"

/* Mozilla includes */
#include <nsCOMPtr.h>
#include <nsWeakReference.h>
#include <nsIObserver.h>
#include <nsStringAPI.h>

#define NS_ESTEID_CONTRACTID "@id.eesti.ee/esteid;1"

typedef nsCOMPtr<nsIEstEIDListener> ListenerPtr;

class nsEstEID : public nsIEstEID,
			     public nsIObserver,
			     public nsSupportsWeakReference
{
public:

  NS_DECL_ISUPPORTS
  NS_DECL_NSIESTEID
  NS_DECL_NSIOBSERVER

  static nsEstEID* getInstance();

  nsEstEID();
  ~nsEstEID();
  nsresult Init();

private:
  enum ListenerType {
	     INSERT_LISTENERS, REMOVE_LISTENERS, MAX_LISTENERS
  };

  nsresult _UpdatePersonalData();
  void _FireListeners(ListenerType, PRUint16);

  // Dynamic data
  vector <std::string> _PersonalData;
  vector <ListenerPtr> _Listeners[MAX_LISTENERS];

  //static PRLock *mLock;

  EstEIDServiceBase* service;

  // Pointer to the service singleton
  static nsEstEID* sEstEID;
};

static const nsCString ListenerNames[] = {
			NS_LITERAL_CSTRING("cardInsert"),
			NS_LITERAL_CSTRING("cardRemove"),
			NS_LITERAL_CSTRING("cardError")
};


#endif // nsEstEID_h
