#if !defined(__RSACRYPT_H_INCLUDED__)
#define __RSACRYPT_H_INCLUDED__

#include <openssl/x509.h>

#include <string>
#include <vector>

#include "../../crypto/signer/Signer.h"
#include "../../io/IOException.h"

namespace digidoc
{
    /**
     * Wrapper for RSA crypto function in OpenSSL. Currently implemented RSA signature
     * verification and signing with private RSA key.
     *
     * @author Janari Põld
     */
    class RSACrypt
    {

      public:
          RSACrypt(X509* cert, RSA* privateKey);
          RSACrypt(X509* cert);
          RSACrypt(RSA* privateKey);
          ~RSACrypt();
          std::vector<unsigned char> sign(const Signer::Digest& digest) throw(IOException);
          bool verify(int digestMethod, std::vector<unsigned char> digest, std::vector<unsigned char> signature) throw(IOException);
          static RSA* loadRSAPrivateKey(const std::string& path) throw(IOException);

      private:
          X509* cert;
          RSA* privateKey;

    };
}

#endif // !defined(__RSACRYPT_H_INCLUDED__)
