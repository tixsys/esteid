#ifndef nsEstEID_h
#define nsEstEID_h

#define ESTEID_DEBUG printf
#define ESTEID_ERROR printf

#include "cardlib.h"

#include "nsIEstEID.h"
#include "nsEstEIDService.h"

/* Mozilla includes */
#include <nsCOMPtr.h>
#include <nsWeakReference.h>
#include <nsIObserver.h>
#include <nsStringAPI.h>

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
	     INSERT_LISTENERS, REMOVE_LISTENERS,
	     READER_LISTENERS, MAX_LISTENERS
  };
  enum CertId {
	  AUTH, SIGN, MAX_CERT
  };

  nsresult _UpdatePersonalData();
  nsresult _UpdateCertData(CertId);
  nsresult _GetCert(nsIEstEIDCertificate **, CertId);
  void _FireListeners(ListenerType, PRUint16);

  // Dynamic data
  vector <std::string> _PersonalData;
  vector <ListenerPtr> _Listeners[MAX_LISTENERS];
  ByteVec _CertData[MAX_CERT];

  //static PRLock *mLock;

  // Pointer to the service singleton
  EstEIDServiceBase* service;
};

static const nsCString ListenerNames[] = {
			NS_LITERAL_CSTRING("OnCardInserted"),
			NS_LITERAL_CSTRING("OnCardRemoved"),
			NS_LITERAL_CSTRING("OnReadersChanged")
};

#define SETNRET(a, b) return b;
#define ESTEID_OK                     SETNRET(0, NS_OK)
#define ESTEID_ERROR_INVALID_ARG      SETNRET(4, NS_ERROR_INVALID_ARG)

#endif // nsEstEID_h
