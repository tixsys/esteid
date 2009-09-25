#if !defined(__X509CERT_H_INCLUDED__)
#define __X509CERT_H_INCLUDED__

#include <openssl/x509.h>

#include "../../io/IOException.h"

namespace digidoc
{
    /**
     * Wrapper for OpenSSL X509 certificate structure.
     */
    class EXP_DIGIDOC X509Cert
    {

      public:
          X509Cert(X509* cert) throw(IOException);
          X509Cert(std::vector<unsigned char> bytes) throw(IOException);
          X509Cert(const X509Cert& copy) throw(IOException);
          ~X509Cert();
          X509Cert& operator=(const X509Cert& copy) throw(IOException);
          X509* getX509() const throw(IOException);
          static X509* copyX509(X509* cert) throw(IOException);
          static X509* loadX509(const std::string& path) throw(IOException);
          static STACK_OF(X509)* loadX509Stack(const std::string& path) throw(IOException);

          std::vector<unsigned char> encodeDER() const throw(IOException);
          long getSerial() const throw(IOException);
          X509_NAME* getIssuerNameAsn1() const;
          std::string getIssuerName() const throw(IOException);
          std::string getSubject() const throw(IOException);
          EVP_PKEY* getPublicKey() const throw(IOException);
          std::vector<unsigned char> getRsaModulus() const throw(IOException);
          std::vector<unsigned char> getRsaExponent() const throw(IOException);

          bool isValid() const throw(IOException);

          int verify(X509_STORE* store = NULL) const throw(IOException);

      protected:
          X509* cert;

      private:
          static std::string toString(X509_NAME* name) throw(IOException);

    };
}

#endif // !defined(__X509CERT_H_INCLUDED__)
