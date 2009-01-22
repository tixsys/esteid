#if !defined(__OCSP_H_INCLUDED__)
#define __OCSP_H_INCLUDED__

#include <openssl/ocsp.h>
#include <openssl/x509.h>

#include <string>

#include "../../io/IOException.h"
#include "OCSPException.h"

namespace digidoc
{
    /**
     * Implements OCSP request to the OCSP server. This class can be used to
     * check whether the certificate is valid or not.
     *
     * If <code>certStore</code> and/or <code>ocspCerts</code> is set, the
     * OCSP response certificate is checked, whether it comes from the correct
     * OCSP server or not.
     *
     * If <code>signCert</code> and <code>signKey</code> is set the OCSP request
     * is signed with the certificate provided.
     *
     * @author Janari Põld
     */
    class OCSP
    {

      public:
          enum CertStatus { GOOD, REVOKED, UNKNOWN };

          OCSP(const std::string& url) throw(IOException);
          ~OCSP();
          void setUrl(const std::string& url) throw(IOException);
          void setCertStore(X509_STORE* certStore);
          void setOCSPCerts(STACK_OF(X509)* ocspCerts);
          void setSignCert(X509* signCert, EVP_PKEY* signKey);
          void setSkew(long skew);
          void setMaxAge(long maxAge);
          CertStatus checkCert(X509* cert, X509* issuer) throw(IOException, OCSPException);

      private:
          void connect() throw(IOException);
          void connectNoSSL() throw(IOException);
          void connectSSL() throw(IOException);
          OCSP_REQUEST* createRequest(X509* cert, X509* issuer) throw(IOException);
          OCSP_RESPONSE* sendRequest(OCSP_REQUEST* req) throw(IOException);
          CertStatus validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer) throw(OCSPException);

          std::string url;
          char* host;
          char* port;
          char* path;
          bool ssl;

          long skew;
          long maxAge;

          BIO* connection;
          X509_STORE* certStore;
          STACK_OF(X509)* ocspCerts;
          X509* signCert;
          EVP_PKEY* signKey;


          // Declare helper structs to automatically release OpenSSL structs after they are out of scope.
      private:
          #define DECLARE_OPENSSL_RELEASE_STRUCT(type) \
          struct type##_scope \
          { \
              type** p; \
              type##_scope(type** p) { this->p = p; } \
              ~type##_scope() { if(p && *p) { type##_free(*p); *p = NULL; } } \
          };

          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_REQUEST)
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_RESPONSE)
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_BASICRESP)
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_CERTID)
          DECLARE_OPENSSL_RELEASE_STRUCT(ASN1_GENERALIZEDTIME)

    };
}

#endif // !defined(__OCSP_H_INCLUDED__)
