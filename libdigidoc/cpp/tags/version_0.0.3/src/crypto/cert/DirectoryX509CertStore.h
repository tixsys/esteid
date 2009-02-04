#if !defined(__DIRECTORYX509CERTSTORE_H_INCLUDED__)
#define __DIRECTORYX509CERTSTORE_H_INCLUDED__

#include <string>
#include <vector>

#include "X509CertStore.h"

namespace digidoc
{
    /**
     * Implementation of X.509 certificate store, which loads the certificates
     * from the directory provided.
     */
    class DirectoryX509CertStore: public X509CertStore
    {

      public:
          DirectoryX509CertStore() throw(IOException);
          DirectoryX509CertStore(const std::string& path) throw(IOException);
          ~DirectoryX509CertStore();

          virtual X509_STORE* getCertStore() const throw(IOException);
          virtual X509* getCert(long certSerial) const throw(IOException);
          virtual X509* getCert(const X509_NAME& subject) const throw(IOException);

      private:
    	  void loadCerts(const std::string& path) throw(IOException);

          std::vector<X509*> certs;

    };
}

#endif // !defined(__DIRECTORYX509CERTSTORE_H_INCLUDED__)
