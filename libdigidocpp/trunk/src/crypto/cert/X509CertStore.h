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

#if !defined(__X509CERTSTORE_H_INCLUDED__)
#define __X509CERTSTORE_H_INCLUDED__

#include "X509Cert.h"

namespace digidoc
{
    /**
     * X.509 certificate store interface.
     */
    class EXP_DIGIDOC X509CertStore
    {

      public:
          virtual ~X509CertStore() {}
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
