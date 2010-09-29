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

#if !defined(__SIGNER_H_INCLUDED__)
#define __SIGNER_H_INCLUDED__

#include "SignException.h"
#include "../../SignatureAttributes.h"

#include <openssl/x509.h>

namespace digidoc
{
    /**
     * Signer interface. Provides methods for signing documents. Digidoc LIB implements EstID signer class
     * that allows signing with EstId chip card. Other implementation may provide signing implementation
     * with other public-key cryptography systems.
     */
    class EXP_DIGIDOC Signer
    {

      public:
          struct EXP_DIGIDOC Digest
          {
              /** Digest type (e.g NID_sha1), available values in openssl/obj_mac.h */
              int type;
              const unsigned char* digest;
              unsigned int length;
          };

          struct EXP_DIGIDOC Signature
          {
              unsigned char* signature;
              unsigned int length;
          };

          virtual X509* getCert() throw(SignException) = 0;
          virtual void sign(const Digest& digest, Signature& signature) throw(SignException) = 0;
          void setSignatureProductionPlace(const SignatureProductionPlace& signatureProductionPlace);
          SignatureProductionPlace getSignatureProductionPlace() const;
          void setSignerRole(const SignerRole& signerRole);
          SignerRole getSignerRole() const;
		  virtual std::string signaturePath() { return std::string(); }

	  protected:
          SignatureProductionPlace signatureProductionPlace;
          SignerRole signerRole;

    };
}

#endif // !defined(__SIGNER_H_INCLUDED__)
