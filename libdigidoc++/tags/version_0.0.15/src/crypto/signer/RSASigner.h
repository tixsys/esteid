#if !defined(__RSASIGNER_H_INCLUDED__)
#define __RSASIGNER_H_INCLUDED__

#include "Signer.h"

#include "../cert/X509Cert.h"

namespace digidoc
{
    /**
     * Implements <code>Signer</code> interface for signing with RSA private key.
     *
     * @author Janari Põld
     */
    class RSASigner : public Signer
    {

      public:
          RSASigner(X509* cert, RSA* privateKey) throw(SignException);
          virtual ~RSASigner();
          X509* getCert() throw(SignException);
          void sign(const Digest& digest, Signature& signature) throw(SignException);

      private:
          X509* cert;
          RSA* privateKey;

    };
}

#endif // !defined(__RSASIGNER_H_INCLUDED__)
