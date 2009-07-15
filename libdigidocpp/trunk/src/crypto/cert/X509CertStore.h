#if !defined(__X509CERTSTORE_H_INCLUDED__)
#define __X509CERTSTORE_H_INCLUDED__

#include <openssl/x509.h>

#include "X509Cert.h"
#include "../../io/IOException.h"

namespace digidoc
{
    /**
     * X.509 certificate store interface.
     */
    class EXP_DIGIDOC X509CertStore
    {

      public:
          static void init(X509CertStore* impl);
          static void destroy();
          static X509CertStore* getInstance() throw(IOException);

          virtual X509_STORE* getCertStore() const throw(IOException) = 0;
          virtual X509* getCert(long certSerial) const throw(IOException) = 0;
          virtual X509* getCert(const X509_NAME& subject) const throw(IOException) = 0;

      private:
          static X509CertStore* INSTANCE;

    };
}

#endif // !defined(__X509CERTSTORE_H_INCLUDED__)
