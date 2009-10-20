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
          enum { Type = 0 };

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

      public:
          virtual X509* getCert() throw(SignException) = 0;
          virtual void sign(const Digest& digest, Signature& signature) throw(SignException) = 0;
          virtual int type() const { return Type; }
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
