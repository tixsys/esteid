#if !defined(__X509CERT_H_INCLUDED__)
#define __X509CERT_H_INCLUDED__

#include <openssl/x509.h>

#include <string>
#include <vector>

#include "../../io/IOException.h"

namespace digidoc
{
    /**
     * Wrapper for OpenSSL X509 certificate structure.
     */
    class X509Cert
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
          std::vector<unsigned char> getRsaModulus() const throw(IOException);
          std::vector<unsigned char> getRsaExponent() const throw(IOException);
          long getCertSerial() const;
          std::string getCertIssuerName() const;

          X509_NAME* getCertIssuerNameAsn1() const;

      protected:
          X509* cert;

      private:
          EVP_PKEY * getPublicKey() const throw(IOException);
          void printErrorMsg() const;

    };
}

#endif // !defined(__X509CERT_H_INCLUDED__)
