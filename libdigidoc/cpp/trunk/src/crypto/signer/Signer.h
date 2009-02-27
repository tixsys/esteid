#if !defined(__SIGNER_H_INCLUDED__)
#define __SIGNER_H_INCLUDED__

#include <openssl/x509.h>

#include <string>
#include <vector>

#include "SignException.h"

namespace digidoc
{
    /**
     * Signer interface. Provides methods for signing documents. Digidoc LIB implements EstID signer class
     * that allows signing with EstId chip card. Other implementation may provide signing implementation
     * with other public-key cryptography systems.
     */
    class Signer
    {

      public:
          struct SignatureProductionPlace
          {
              SignatureProductionPlace();
              SignatureProductionPlace(std::string city, std::string stateOrProvince, std::string postalCode, std::string countryName);
              bool isEmpty();

              std::string city;
              std::string stateOrProvince;
              std::string postalCode;
              std::string countryName;
          };

          struct SignerRole
          {
              SignerRole();
              SignerRole(const std::string& claimedRole);
              bool isEmpty();

              typedef std::vector<std::string> TRoles;
              TRoles claimedRoles;
          };

          struct Digest
          {
              /** Digest type (e.g NID_sha1), available values in openssl/obj_mac.h */
              int type;
              const unsigned char* digest;
              unsigned int length;
          };

          struct Signature
          {
              unsigned char* signature;
              unsigned int length;
          };

      public:
          virtual X509* getCert() throw(SignException) = 0;
          virtual void sign(const Digest& digest, Signature& signature) throw(SignException) = 0;
          void setSignatureProductionPlace(const SignatureProductionPlace& signatureProductionPlace);
          SignatureProductionPlace getSignatureProductionPlace() const;
          void setSignerRole(const SignerRole& signerRole);
          SignerRole getSignerRole() const;

      protected:
          SignatureProductionPlace signatureProductionPlace;
          SignerRole signerRole;

    };
}

#endif // !defined(__SIGNER_H_INCLUDED__)
