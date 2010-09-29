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

#if !defined(__DIRECTORYX509CERTSTORE_H_INCLUDED__)
#define __DIRECTORYX509CERTSTORE_H_INCLUDED__

#include "X509CertStore.h"

namespace digidoc
{
    /**
     * Implementation of X.509 certificate store, which loads the certificates
     * from the directory provided.
     */
    class EXP_DIGIDOC DirectoryX509CertStore: public X509CertStore
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
