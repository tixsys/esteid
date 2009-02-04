#if !defined(__OCSP_H_INCLUDED__)
#define __OCSP_H_INCLUDED__

#include <openssl/ocsp.h>
#include <openssl/x509.h>

#include <string>
#include <time.h>

#include "../../io/IOException.h"
#include "../OpenSSLHelpers.h"
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
          void setOCSPCerts(STACK_OF(X509)* ocspCerts);
          void setCertStore(X509_STORE* certStore);
          void setSignCert(X509* signCert, EVP_PKEY* signKey);
          void setSkew(long skew);
          void setMaxAge(long maxAge);
          CertStatus checkCert(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce) throw(IOException, OCSPException);
          CertStatus checkCert(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce,
        		  std::vector<unsigned char>& ocspResponseDER, tm& producedAt) throw(IOException, OCSPException);

          void verifyResponse( const std::vector<unsigned char>& ocspResponseDER) const;
          std::vector<unsigned char> getNonce(const std::vector<unsigned char>& ocspResponseDER) const;

      private:
    	  CertStatus checkCert(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce,
    			  OCSP_REQUEST** req, OCSP_RESPONSE** resp) throw(IOException, OCSPException);
          void connect() throw(IOException);
          void connectNoSSL() throw(IOException);
          void connectSSL() throw(IOException);
          OCSP_REQUEST* createRequest(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce) throw(IOException);
          OCSP_RESPONSE* sendRequest(OCSP_REQUEST* req) throw(IOException);
          CertStatus validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer) throw(OCSPException);

          static tm convert(ASN1_GENERALIZEDTIME* time) throw(IOException);

          std::string url;
          char* host;
          char* port;
          char* path;
          bool ssl;

          long skew;
          long maxAge;

          BIO* connection;
          X509* ocspCert;
          STACK_OF(X509)* ocspCerts;
          X509_STORE* certStore;
          X509* signCert;
          EVP_PKEY* signKey;


          // Declare helper structs to automatically release OpenSSL structs after they are out of scope.
      private:
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_REQUEST)
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_RESPONSE)
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_BASICRESP)
          DECLARE_OPENSSL_RELEASE_STRUCT(OCSP_CERTID)
          DECLARE_OPENSSL_RELEASE_STRUCT(BIO)
          DECLARE_OPENSSL_RELEASE_STRUCT(X509_EXTENSION)

    };
}

#endif // !defined(__OCSP_H_INCLUDED__)
