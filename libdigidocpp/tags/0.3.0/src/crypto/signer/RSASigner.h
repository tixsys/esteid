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

#if !defined(__RSASIGNER_H_INCLUDED__)
#define __RSASIGNER_H_INCLUDED__

#include "Signer.h"

#include "../cert/X509Cert.h"

namespace digidoc
{
    /**
     * Implements <code>Signer</code> interface for signing with RSA private key.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC RSASigner : public Signer
    {

      public:
          RSASigner(X509* cert, RSA* privateKey) throw(SignException);
          virtual ~RSASigner();
          X509* getCert() throw(SignException);
          void sign(const Digest& digest, Signature& signature) throw(SignException);

      private:
          X509* cert;
          RSA* privateKey;

    };
}

#endif // !defined(__RSASIGNER_H_INCLUDED__)
