#include <openssl/err.h>
#include <openssl/objects.h>

#include "../log.h"
#include "../util/String.h"
#include "Digest.h"

const std::string digidoc::Digest::URI_SHA1 = "http://www.w3.org/2000/09/xmldsig#sha1";
const std::string digidoc::Digest::URI_SHA256 = "http://www.w3.org/2001/04/xmlenc#sha256";

/**
 * Digest factory, creates instance of digest implementation based on digest method.
 *
 * @param method digest method (e.g NID_sha1 for SHA1, see openssl/obj_mac.h).
 * @return returns instance of digest implementation.
 * @throws IOException throws exception if digest method is not supported.
 */
std::auto_ptr<digidoc::Digest> digidoc::Digest::create(int method) throw(IOException)
{
    switch(method)
    {
        case NID_sha1:   { return std::auto_ptr<Digest>(new SHA1Digest);   } break;
        case NID_sha256: { return std::auto_ptr<Digest>(new SHA256Digest); } break;
    }

    THROW_IOEXCEPTION("Digest method '%s' is not supported.", OBJ_nid2sn(method));
}

/**
 * Digest factory, creates instance of digest implementation based on digest method URI.
 * For available method URIs see:
 * <li>
 *   <ul><b>W3C XML Encryption Syntax and Processing</b> (10 December 2005) http://www.w3.org/TR/xmlenc-core/</ul>
 *   <ul><b>RFC 4051</b> http://www.ietf.org/rfc/rfc4051.txt</ul>
 * </li>
 *
 * @param methodUri digest method URI (e.g. 'http://www.w3.org/2000/09/xmldsig#sha1' for SHA1).
 * @return returns instance of digest implementation.
 * @throws IOException throws exception if digest method is not supported.
 */
std::auto_ptr<digidoc::Digest> digidoc::Digest::create(const std::string& methodUri) throw(IOException)
{
    return create(toMethod(methodUri));
}

/**
 * Converts digest method URI to OpenSSL method id (e.g. 'http://www.w3.org/2000/09/xmldsig#sha1' to NID_sha1,
 * see openssl/obj_mac.h)
 * For available method URIs see:
 * <li>
 *   <ul><b>W3C XML Encryption Syntax and Processing</b> (10 December 2005) http://www.w3.org/TR/xmlenc-core/</ul>
 *   <ul><b>RFC 4051</b> http://www.ietf.org/rfc/rfc4051.txt</ul>
 * </li>
 *
 * @param methodUri digest method URI (e.g. 'http://www.w3.org/2000/09/xmldsig#sha1' for SHA1).
 * @return returns digest OpenSSL method id.
 * @throws IOException throws exception if digest method is not supported.
 */
int digidoc::Digest::toMethod(const std::string& methodUri) throw(IOException)
{
    if(methodUri == URI_SHA1)        { return NID_sha1;   }
    else if(methodUri == URI_SHA256) { return NID_sha256; }

    THROW_IOEXCEPTION("Digest method URI '%s' is not supported.", methodUri.c_str());
}

/**
 * SHA1 digest size.
 */
const unsigned int digidoc::SHA1Digest::DIGEST_SIZE = 20;

/**
 * Initializes OpenSSL SHA1 digest calculator.
 *
 * @throws IOException throws exception if the SHA1 digest calculator initialization failed.
 */
digidoc::SHA1Digest::SHA1Digest() throw(IOException)
{
    if(SHA1_Init(&ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to initialize SHA1 digest calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Destroys OpenSSL SHA1 digest calculator.
 */
digidoc::SHA1Digest::~SHA1Digest()
{
    unsigned char digest[DIGEST_SIZE];
    if(SHA1_Final(digest, &ctx) != 1)
    {
        ERR("Failed to uninitialize SHA1 calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Add data for digest calculation. After calling <code>getDigest()</code> SHA1 context
 * is uninitialized and this method should not be called.
 *
 * @param data data to add for digest calculation.
 * @param length length of the data.
 * @throws IOException throws exception if SHA1 update failed.
 * @see getDigest()
 */
void digidoc::SHA1Digest::update(const unsigned char* data, unsigned long length) throw(IOException)
{
    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.")
    }

    if(SHA1_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA1 digest value: %s", ERR_reason_error_string(ERR_get_error()))
    }
}

/**
 * Calculate message digest. SHA context will be invalid after this call.
 * For calculating an other digest you must create new SHA1Digest class.
 *
 * @return returns the calculated SHA1 digest.
 * @throws IOException throws exception if SHA1 update failed.
 */
std::vector<unsigned char> digidoc::SHA1Digest::getDigest() throw(IOException)
{
    // If digest is already calculated return it.
    if(!digest.empty())
    {
        return digest;
    }

    unsigned char buf[DIGEST_SIZE];
    if(SHA1_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA1 digest: %s", ERR_reason_error_string(ERR_get_error()))
    }

    for(unsigned int i = 0; i < DIGEST_SIZE; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}

/**
 * @return returns size of the digest.
 */
int digidoc::SHA1Digest::getSize() const
{
    return DIGEST_SIZE;
}

/**
 * @return returns digest method OpenSSL NID. See openssl/obj_mac.h for available methods.
 */
int digidoc::SHA1Digest::getMethod() const
{
    return NID_sha1;
}

/**
 * return returns digest method URI.
 * @see Digest::toMethod(const std::string& methodUri)
 */
std::string digidoc::SHA1Digest::getUri() const
{
    return URI_SHA1;
}

/**
 * SHA256 digest size.
 */
const unsigned int digidoc::SHA256Digest::DIGEST_SIZE = 32;

/**
 * Initializes OpenSSL SHA256 digest calculator.
 *
 * @throws IOException throws exception if the SHA256 digest calculator initialization failed.
 */
digidoc::SHA256Digest::SHA256Digest() throw(IOException)
{
    if(SHA256_Init(&ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to initialize SHA256 digest calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Destroys OpenSSL SHA256 digest calculator.
 */
digidoc::SHA256Digest::~SHA256Digest()
{
    unsigned char digest[DIGEST_SIZE];
    if(SHA256_Final(digest, &ctx) != 1)
    {
        ERR("Failed to uninitialize SHA256 calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Add data for digest calculation. After calling <code>getDigest()</code> SHA256 context
 * is uninitialized and this method should not be called.
 *
 * @param data data to add for digest calculation.
 * @param length length of the data.
 * @throws IOException throws exception if SHA256 update failed.
 * @see getDigest()
 */
void digidoc::SHA256Digest::update(const unsigned char* data, unsigned long length) throw(IOException)
{
    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.")
    }

    if(SHA256_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA256 digest value: %s", ERR_reason_error_string(ERR_get_error()))
    }
}

/**
 * Calculate message digest. SHA context will be invalid after this call.
 * For calculating an other digest you must create new SHA256Digest class.
 *
 * @return returns the calculated SHA256 digest.
 * @throws IOException throws exception if SHA256 update failed.
 */
std::vector<unsigned char> digidoc::SHA256Digest::getDigest() throw(IOException)
{
    // If digest is already calculated return it.
    if(!digest.empty())
    {
        return digest;
    }

    unsigned char buf[DIGEST_SIZE];
    if(SHA256_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA256 digest: %s", ERR_reason_error_string(ERR_get_error()))
    }

    for(unsigned int i = 0; i < DIGEST_SIZE; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}

/**
 * @return returns size of the digest.
 */
int digidoc::SHA256Digest::getSize() const
{
    return DIGEST_SIZE;
}

/**
 * @return returns digest method OpenSSL NID. See openssl/obj_mac.h for available methods.
 */
int digidoc::SHA256Digest::getMethod() const
{
    return NID_sha256;
}

/**
 * return returns digest method URI.
 * @see Digest::toMethod(const std::string& methodUri)
 */
std::string digidoc::SHA256Digest::getUri() const
{
    return URI_SHA256;
}
