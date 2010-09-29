/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if !defined(__OCSP_H_INCLUDED__)
#define __OCSP_H_INCLUDED__

#include "../../io/IOException.h"
#include "../OpenSSLHelpers.h"
#include "OCSPException.h"

#include <openssl/ocsp.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>

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

          OCSP( const std::string &url ) throw(IOException);
          ~OCSP();
          void setUrl( const std::string& url ) throw(IOException);
          void setOCSPCerts(STACK_OF(X509)* ocspCerts);
          void setCertStore(X509_STORE* certStore);
          void setSignCert(X509* signCert, EVP_PKEY* signKey);
          void setSkew(long skew);
          void setMaxAge(long maxAge);
          CertStatus checkCert(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce) throw(IOException, OCSPException);
          CertStatus checkCert(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce,
                  std::vector<unsigned char>& ocspResponseDER, tm& producedAt) throw(IOException, OCSPException);

          void verifyResponse( const std::vector<unsigned char>& ocspResponseDER) const throw(IOException);
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

          std::string url, host, port, path;
          bool ssl;

          long skew;
          long maxAge;

          BIO *connection;
          SSL_CTX *ctx;
          X509* ocspCert;
          STACK_OF(X509)* ocspCerts;
          X509_STORE* certStore;
          X509* signCert;
          EVP_PKEY* signKey;
    };
}

#endif // !defined(__OCSP_H_INCLUDED__)
