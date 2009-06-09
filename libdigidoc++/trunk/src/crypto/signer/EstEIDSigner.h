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
          virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException);

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
