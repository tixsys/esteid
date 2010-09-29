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

#if !defined(__ESTEIDSIGNER_H_INCLUDED__)
#define __ESTEIDSIGNER_H_INCLUDED__

#include "PKCS11Signer.h"

namespace digidoc
{
    /**
     * Implements EstEID ID-Card signer. Selects the correct certificate for signing by
     * implementing abstract method <code>selectSigningCertificate</code>, which select
     * the correct certificate by certificate label (label is 'Allkirjastamine').
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC EstEIDSigner : public PKCS11Signer
    {

      public:
          EstEIDSigner(const std::string& driver) throw(SignException);
          virtual ~EstEIDSigner();

      protected:
          virtual PKCS11Cert selectSigningCertificate(std::vector<PKCS11Cert> certificates) throw(SignException);

    };

    /**
     * EstEID ID-Card signer with interactive PIN acquisition from console.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC EstEIDConsolePinSigner : public EstEIDSigner
    {

      public:
          EstEIDConsolePinSigner(const std::string& driver, const std::string& prompt) throw(SignException);
          EstEIDConsolePinSigner(const std::string& driver) throw(SignException);
          virtual ~EstEIDConsolePinSigner();
          void setPrompt(const std::string& prompt);

      protected:
          virtual std::string getPin(PKCS11Cert certificate) throw(SignException);

      private:
          std::string prompt;

    };
}

#endif // !defined(__ESTEIDSIGNER_H_INCLUDED__)
