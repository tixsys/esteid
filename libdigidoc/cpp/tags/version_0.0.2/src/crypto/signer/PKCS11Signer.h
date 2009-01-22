#if !defined(__PKCS11SIGNER_H_INCLUDED__)
#define __PKCS11SIGNER_H_INCLUDED__

#include <libp11.h>

#include <string>

#include "Signer.h"

namespace digidoc
{
    /**
     * Implements <code>Signer</code> interface for ID-Cards, which support PKCS #11 protocol.
     * Callback function <code>selectSignCertificate</code> is called if there are more than 1
     * certificates available on the ID-Card. Parameters <code>certificates</code> provide list
     * of all certificates found in the ID-Card.
     * Callback function <code>getPin</code> is called if the selected certificate needs PIN
     * to login.
     *
     * @author Janari Põld
     */
    class PKCS11Signer : public Signer
    {

      public:
    	  struct PKCS11Token { std::string label; std::string manufacturer; std::string model; std::string serialNr; };
    	  struct PKCS11Cert { PKCS11Token token; std::string label; X509* cert; };
    	  PKCS11Signer(const std::string& driver, X509* (*selectSignCertificate)(std::vector<PKCS11Cert> certificates),
    			  std::string (*getPin)(PKCS11Cert certificate)) throw(SignException);
    	  ~PKCS11Signer();
          X509* getCert() throw(SignException);
          void sign(const Digest& digest, Signature& signature) throw(SignException);

      private:
    	  PKCS11Cert createPKCS11Cert(PKCS11_SLOT* slot, PKCS11_CERT* cert);

    	  std::string driver;
    	  X509* (*selectSignCertificate)(std::vector<PKCS11Cert> certificates);
    	  std::string (*getPin)(PKCS11Cert certificate);
    	  PKCS11_CTX* ctx;
    	  PKCS11_CERT* signCertificate;
    	  PKCS11_SLOT* signSlot;

    	  PKCS11_SLOT* slots;
    	  unsigned int numberOfSlots;

    };
}

#endif // !defined(__PKCS11SIGNER_H_INCLUDED__)
