#include "../log.h"
#include "../util/String.h"
#include "SHA1Digest.h"


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
		THROW_IOEXCEPTION("Failed to initialize SHA1 digest calculator.");
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
        ERROR("Failed to uninitialize SHA1 calculator.")
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
	if(SHA1_Update(&ctx, static_cast<const void*>(data), length) != 1)
	{
        THROW_IOEXCEPTION("Failed to update SHA1 digest value.")
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
    unsigned char buf[DIGEST_SIZE];
    if(SHA1_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA1 digest.")
    }

	std::vector<unsigned char> digest;
	for(unsigned int i = 0; i < DIGEST_SIZE; i++)
	{
		digest.push_back(buf[i]);
	}

	return digest;
}
