#if !defined(__SHA1DIGEST_H_INCLUDED__)
#define __SHA1DIGEST_H_INCLUDED__

#include <openssl/sha.h>

#include <vector>

#include "../io/IOException.h"

namespace digidoc
{
    /**
     * Calculate SHA1 digest (hash) from data.
     */
    class SHA1Digest
    {

      public:
          SHA1Digest() throw(IOException);
          ~SHA1Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);

      private:
          SHA_CTX ctx;
          static const unsigned int DIGEST_SIZE;

    };
}

#endif // !defined(__SHA1DIGEST_H_INCLUDED__)
