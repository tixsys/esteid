#if !defined(__DIGEST_H_INCLUDED__)
#define __DIGEST_H_INCLUDED__

#include <openssl/sha.h>

#include <memory>
#include <vector>

#include "../io/IOException.h"

namespace digidoc
{
    /**
     * Digest calculation interface.
     */
    class Digest
    {

      public:
          virtual void update(const unsigned char* data, unsigned long length) throw(IOException) = 0;
          virtual std::vector<unsigned char> getDigest() throw(IOException) = 0;
          virtual int getSize() const = 0;
          virtual int getMethod() const = 0;
          virtual std::string getName() const = 0;
          virtual std::string getUri() const = 0;

          static std::auto_ptr<Digest> create() throw(IOException);
          static std::auto_ptr<Digest> create(int method) throw(IOException);
          static std::auto_ptr<Digest> create(const std::string& methodUri) throw(IOException);
          static int toMethod(const std::string& methodUri) throw(IOException);

          static const std::string URI_SHA1;
          static const std::string URI_SHA256;

    };

    /**
     * Calculate SHA1 digest (hash) from data.
     */
    class SHA1Digest : public Digest
    {

      public:
          SHA1Digest() throw(IOException);
          ~SHA1Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);
          int getSize() const;
          int getMethod() const;
          std::string getName() const;
          std::string getUri() const;

          static const unsigned int DIGEST_SIZE;

      private:
          SHA_CTX ctx;
          std::vector<unsigned char> digest;

    };

    /**
     * Calculate SHA256 digest (hash) from data.
     */
    class SHA256Digest : public Digest
    {

      public:
          SHA256Digest() throw(IOException);
          ~SHA256Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);
          int getSize() const;
          int getMethod() const;
          std::string getName() const;
          std::string getUri() const;

          static const unsigned int DIGEST_SIZE;

      private:
          SHA256_CTX ctx;
          std::vector<unsigned char> digest;

    };
}

#endif // !defined(__DIGEST_H_INCLUDED__)
