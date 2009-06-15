#if !defined(__SIGNATUREBES_H_INCLUDED__)
#define __SIGNATUREBES_H_INCLUDED__

#include <string>

#include "Signature.h"

namespace digidoc
{
    class SignatureBES : public Signature
    {

      public:
          SignatureBES();
          SignatureBES(const std::string& path) throw(SignatureException);
          virtual std::string getMediaType() const;

          static const std::string MEDIA_TYPE;

      protected:
          virtual void sign(Signer* signer);

      private:
          void checkIfWellFormed() throw(SignatureException);

    };
}

#endif // !defined(__SIGNATUREBES_H_INCLUDED__)
