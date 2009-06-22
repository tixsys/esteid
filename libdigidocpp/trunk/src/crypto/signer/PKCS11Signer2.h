#if !defined(__PKCS11SIGNER2_H_INCLUDED__)
#define __PKCS11SIGNER2_H_INCLUDED__

#include <string>

#include "PKCS11Signer.h"

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
    class PKCS11Signer2Private;
    class EXP_DIGIDOC PKCS11Signer2 : public Signer
    {

      public:
          PKCS11Signer2() throw(SignException);
          PKCS11Signer2(const std::string& driver) throw(SignException);
          virtual ~PKCS11Signer2();
          X509* getCert() throw(SignException);
          void sign(const Digest& digest, Signature& signature) throw(SignException);
          void unloadDriver();

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
		  virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException) = 0;

      public:
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
		  virtual std::string getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException) = 0;

      private:
          void loadDriver(const std::string& driver) throw(SignException);

          PKCS11Signer2Private *d;
    };
}

#endif // !defined(__PKCS11SIGNER2_H_INCLUDED__)
