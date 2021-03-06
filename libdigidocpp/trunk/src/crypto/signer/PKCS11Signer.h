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

#if !defined(__PKCS11SIGNER_H_INCLUDED__)
#define __PKCS11SIGNER_H_INCLUDED__

#include "Signer.h"

namespace digidoc
{

    /**
     * Implements <code>Signer</code> interface for ID-Cards, which support PKCS #11 protocol.
     *
     * Abstract method <code>selectSigningCertificate</code> is called if the signer needs
     * to choose the correct signing certificate. It is called also if there is only one certificate
     * found on ID-Card. Parameter <code>certificates</code> provide list of all certificates
     * found in the ID-Card.
     *
     * Abstract method <code>getPin</code> is called if the selected certificate needs PIN
     * to login.
     *
     * @see X509* selectSignCertificate(std::vector<PKCS11Cert> certificates) throw(SignException)
     * @see std::string getPin(PKCS11Cert certificate) throw(SignException)
     *
     * @author Janari Põld
     */
	class PKCS11SignerPrivate;
    class EXP_DIGIDOC PKCS11Signer : public Signer
    {

      public:
          struct PKCS11Token { std::string label, manufacturer, model, serialNr; };
          struct PKCS11Cert { PKCS11Token token; std::string label; X509* cert; };

          PKCS11Signer() throw(SignException);
          PKCS11Signer(const std::string& driver) throw(SignException);
          virtual ~PKCS11Signer();
          X509* getCert() throw(SignException);
          void sign(const Digest& digest, Signature& signature) throw(SignException);

          void loadDriver() throw(SignException);
          void loadDriver(const std::string& driver) throw(SignException);
          void unloadDriver();

          /**
           * Abstract method that returns PIN code for the selected signing certificate.
           * If PIN code is not needed this method is never called. To cancel the login
           * this method should throw an exception.
           *
           * @param certificate certificate that is used for signing and needs a PIN
           *        for login.
           * @return returns the PIN code to login.
           * @throws SignException should throw an exception if the login operation
           *         should be canceled.
           */
          virtual std::string getPin(PKCS11Cert certificate) throw(SignException) = 0;

      protected:

          /**
           * Abstract method for selecting the correct signing certificate. If none of
           * the certificates suit for signing, this method should throw an SignException.
           * This method is always called, when there are at least 1 certificates available.
           *
           * @param certificates available certificates to choose from.
           * @return returns the certificate used for signing.
           * @throws SignException should throw an exception if no suitable certificate
           *         is in the list or the operation should be canceled.
           */
          virtual PKCS11Cert selectSigningCertificate(std::vector<PKCS11Cert> certificates) throw(SignException) = 0;

          virtual void showPinpad() {};
          virtual void hidePinpad() {};

      private:
          PKCS11SignerPrivate *d;
    };
}

#endif // !defined(__PKCS11SIGNER_H_INCLUDED__)
