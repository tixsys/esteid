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

#if !defined(__X509CERT_H_INCLUDED__)
#define __X509CERT_H_INCLUDED__

#include "../../io/IOException.h"

#include <openssl/x509.h>

namespace digidoc
{
    /**
     * Wrapper for OpenSSL X509 certificate structure.
     */
    class EXP_DIGIDOC X509Cert
    {

      public:
          ~X509Cert();
          X509Cert();
          X509Cert(const std::string& pem) throw(IOException);
#ifndef SWIG
          X509Cert(std::vector<unsigned char> bytes) throw(IOException);
          X509Cert(X509* cert) throw(IOException);
          X509Cert(const X509Cert& copy) throw(IOException);
          X509Cert& operator=(const X509Cert& copy) throw(IOException);
          X509* getX509() const throw(IOException);
          static X509* copyX509(X509* cert) throw(IOException);
          static X509* loadX509(const std::string& path) throw(IOException);
          static STACK_OF(X509)* loadX509Stack(const std::string& path) throw(IOException);
          X509_NAME* getIssuerNameAsn1() const;
          EVP_PKEY* getPublicKey() const throw(IOException);
          int verify(X509_STORE* store = NULL) const throw(IOException);
#endif
          std::vector<unsigned char> encodeDER() const throw(IOException);
          long getSerial() const throw(IOException);
          std::string getIssuerName() const throw(IOException);
          std::string getSubjectName() const throw(IOException);
          std::string getSubjectInfo(const std::string& ln) const throw(IOException);
          std::string getIssuerInfo(const std::string& ln) const throw(IOException);
          std::vector<unsigned char> getRsaModulus() const throw(IOException);
          std::vector<unsigned char> getRsaExponent() const throw(IOException);
          bool isValid() const throw(IOException);
          int compareIssuerToString(const std::string&) const throw(IOException);

      protected:
          X509* cert;

      private:
          static std::string toString(X509_NAME* name) throw(IOException);

    };
}

#endif // !defined(__X509CERT_H_INCLUDED__)
