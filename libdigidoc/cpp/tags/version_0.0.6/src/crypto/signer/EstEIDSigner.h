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
    class EstEIDSigner : public PKCS11Signer
    {

      public:
          EstEIDSigner(const std::string& driver) throw(SignException);
          virtual ~EstEIDSigner();

      protected:
          virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException);

    };

    /**
     * EstEID ID-Card signer with interactive acquisition from console
     */
    class EstEIDConsolePinSigner : public EstEIDSigner
    {

      public:
          EstEIDConsolePinSigner(const std::string& driver) throw(SignException);
          virtual ~EstEIDConsolePinSigner();

      protected:
          //virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException);
          virtual std::string getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException);

      private:
          void printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert);

    };
}

#endif // !defined(__ESTEIDSIGNER_H_INCLUDED__)
