#ifndef nsEstEIDCertificate_h
#define nsEstEIDCertificate_h

#include "nsEstEID.h"

/* Mozilla includes */
#include <nsCOMPtr.h>
#include "nsIX509Cert.h"
#include "nsISecurityCheckedComponent.h"

class nsEstEIDCertificate : public nsIEstEIDCertificate,
                            public nsISecurityCheckedComponent
{
public:

  NS_DECL_ISUPPORTS
  NS_DECL_NSIESTEIDCERTIFICATE
  NS_DECL_NSISECURITYCHECKEDCOMPONENT

  nsEstEIDCertificate();
  ~nsEstEIDCertificate();

  nsresult Decode(ByteVec);

private:
	nsIX509Cert *mCert;
	char *mBase64;
};

#endif /* nsEstEIDCertificate_h */
