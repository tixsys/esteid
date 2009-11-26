#if !defined(__DIGEST_H_INCLUDED__)
#define __DIGEST_H_INCLUDED__

#include "../io/IOException.h"

#include <memory>
#include <openssl/sha.h>

#define URI_SHA1 "http://www.w3.org/2000/09/xmldsig#sha1"
#define URI_SHA224 "http://www.w3.org/2001/04/xmldsig-more#sha224"
#define URI_SHA256 "http://www.w3.org/2001/04/xmlenc#sha256"
#define URI_SHA384 "http://www.w3.org/2001/04/xmldsig-more#sha384"
#define URI_SHA512 "http://www.w3.org/2001/04/xmlenc#sha512"

namespace digidoc
{
    /**
     * Digest calculation interface.
     */
    class EXP_DIGIDOC Digest
    {
      public:
          virtual ~Digest() {}
          void update(std::vector<unsigned char> data) throw(IOException);
          virtual void update(const unsigned char* data, unsigned long length) throw(IOException) = 0;
          virtual std::vector<unsigned char> getDigest() throw(IOException) = 0;
          unsigned int getSize() const;
          int getMethod() const;
          std::string getName() const;
          std::string getUri() const;

          static std::auto_ptr<Digest> create() throw(IOException);
          static std::auto_ptr<Digest> create(int method) throw(IOException);
          static std::auto_ptr<Digest> create(const std::string& methodUri) throw(IOException);
          static int toMethod(const std::string& methodUri) throw(IOException);
          static bool isSupported(const std::string& methodUri);

      protected:
          int method;
          std::vector<unsigned char> digest;
    };

    /**
     * Calculate SHA1 digest (hash) from data.
     */
    class EXP_DIGIDOC SHA1Digest : public Digest
    {
      public:
          SHA1Digest() throw(IOException);
          virtual ~SHA1Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);

      private:
          SHA_CTX ctx;
    };

    /**
      * Calculate SHA224 digest (hash) from data.
      */
    class EXP_DIGIDOC SHA224Digest : public Digest
    {
      public:
          SHA224Digest() throw(IOException);
          virtual ~SHA224Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);

      private:
          SHA256_CTX ctx;
    };

    /**
     * Calculate SHA256 digest (hash) from data.
     */
    class EXP_DIGIDOC SHA256Digest : public Digest
    {
      public:
          SHA256Digest() throw(IOException);
          virtual ~SHA256Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);

      private:
          SHA256_CTX ctx;
    };

    /**
      * Calculate SHA384 digest (hash) from data.
      */
    class EXP_DIGIDOC SHA384Digest : public Digest
    {
      public:
          SHA384Digest() throw(IOException);
          virtual ~SHA384Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);

      private:
          SHA512_CTX ctx;
    };

    /**
      * Calculate SHA512 digest (hash) from data.
      */
    class EXP_DIGIDOC SHA512Digest : public Digest
    {
      public:
          SHA512Digest() throw(IOException);
          virtual ~SHA512Digest();
          void update(const unsigned char* data, unsigned long length) throw(IOException);
          std::vector<unsigned char> getDigest() throw(IOException);

      private:
          SHA512_CTX ctx;
    };
}

#endif // !defined(__DIGEST_H_INCLUDED__)
