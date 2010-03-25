/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <openssl/err.h>
#include <openssl/objects.h>

#include "../log.h"
#include "../Conf.h"
#include "../util/String.h"
#include "Digest.h"

/**
 * Add data for digest calculation.
 *
 * @param data data to add for digest calculation.
 * @throws IOException throws exception if SHA1 update failed.
 * @see update(const unsigned char* data, unsigned long length)
 */
void digidoc::Digest::update(std::vector<unsigned char> data) throw(IOException)
{
    update(&data[0], data.size());
}

/**
 * Digest factory, creates instance of digest implementation based on configuration.
 *
 * @return returns instance of digest implementation.
 * @throws IOException throws exception if digest method is not supported.
 */
std::auto_ptr<digidoc::Digest> digidoc::Digest::create() throw(IOException)
{
    return create(Conf::getInstance()->getDigestUri());
}

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
    default:
        THROW_IOEXCEPTION("Digest method '%s' is not supported.", OBJ_nid2sn(method));
    case NID_sha1: return std::auto_ptr<Digest>(new SHA1Digest);
    case NID_sha224: return std::auto_ptr<Digest>(new SHA224Digest);
    case NID_sha256: return std::auto_ptr<Digest>(new SHA256Digest);
    case NID_sha384: return std::auto_ptr<Digest>(new SHA384Digest);
    case NID_sha512: return std::auto_ptr<Digest>(new SHA512Digest);
    }
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
 * @return returns digest method OpenSSL NID. See openssl/obj_mac.h for available methods.
 */
int digidoc::Digest::getMethod() const
{
    return method;
}

/**
 * @return returns digest method name.
 */
std::string digidoc::Digest::getName() const
{
    return OBJ_nid2sn(method);
}

/**
 * @return returns size of the digest.
 */
unsigned int digidoc::Digest::getSize() const
{
    switch(method)
    {
    case NID_sha1: return SHA_DIGEST_LENGTH;
    case NID_sha224: return SHA224_DIGEST_LENGTH;
    case NID_sha256: return SHA256_DIGEST_LENGTH;
    case NID_sha384: return SHA384_DIGEST_LENGTH;
    case NID_sha512: return SHA512_DIGEST_LENGTH;
    default: return 0;
    }
}

/**
 * return returns digest method URI.
 * @see Digest::toMethod(const std::string& methodUri)
 */
std::string digidoc::Digest::getUri() const
{
    switch(method)
    {
    case NID_sha1: return URI_SHA1;
    case NID_sha224: return URI_SHA224;
    case NID_sha256: return URI_SHA256;
    case NID_sha384: return URI_SHA384;
    case NID_sha512: return URI_SHA512;
    default: return "";
    }
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
    if ( methodUri == URI_SHA1 ) return NID_sha1;
    if ( methodUri == URI_SHA224 ) return NID_sha224;
    if ( methodUri == URI_SHA256 ) return NID_sha256;
    if ( methodUri == URI_SHA384 ) return NID_sha384;
    if ( methodUri == URI_SHA512 ) return NID_sha512;
    THROW_IOEXCEPTION( "Digest method URI '%s' is not supported.", methodUri.c_str() );
    return 0;
}

/**
 * Check if methodUri is supported by Digest.create(methodUri) method
 * For available method URIs see:
 * <li>
 *   <ul><b>W3C XML Encryption Syntax and Processing</b> (10 December 2005) http://www.w3.org/TR/xmlenc-core/</ul>
 *   <ul><b>RFC 4051</b> http://www.ietf.org/rfc/rfc4051.txt</ul>
 * </li>
 * @param methodUri igest method URI (e.g. 'http://www.w3.org/2000/09/xmldsig#sha1' for SHA1).
 * @return if methodUri is supported
 */
bool digidoc::Digest::isSupported(const std::string& methodUri)
{
    try
    {
        toMethod(methodUri);
        return true;
    }
    catch (IOException& )
    {
        return false;
    }
}

/**
 * Initializes OpenSSL SHA1 digest calculator.
 *
 * @throws IOException throws exception if the SHA1 digest calculator initialization failed.
 */
digidoc::SHA1Digest::SHA1Digest() throw(IOException)
{
    method = NID_sha1;
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
    unsigned char digest[SHA_DIGEST_LENGTH];
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
    if(data == NULL)
    {
        THROW_IOEXCEPTION("Can not update digest value from NULL pointer.");
    }

    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.");
    }

    if(SHA1_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA1 digest value: %s", ERR_reason_error_string(ERR_get_error()));
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

    unsigned char buf[SHA_DIGEST_LENGTH];
    if(SHA1_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA1 digest: %s", ERR_reason_error_string(ERR_get_error()));
    }

    for(unsigned int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}

/**
 * Initializes OpenSSL SHA224 digest calculator.
 *
 * @throws IOException throws exception if the SHA224 digest calculator initialization failed.
 */
digidoc::SHA224Digest::SHA224Digest() throw(IOException)
{
    method = NID_sha224;
    if(SHA224_Init(&ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to initialize SHA224 digest calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Destroys OpenSSL SHA224 digest calculator.
 */
digidoc::SHA224Digest::~SHA224Digest()
{
    unsigned char digest[SHA224_DIGEST_LENGTH];
    if(SHA224_Final(digest, &ctx) != 1)
    {
        ERR("Failed to uninitialize SHA224 calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Add data for digest calculation. After calling <code>getDigest()</code> SHA224 context
 * is uninitialized and this method should not be called.
 *
 * @param data data to add for digest calculation.
 * @param length length of the data.
 * @throws IOException throws exception if SHA224 update failed.
 * @see getDigest()
 */
void digidoc::SHA224Digest::update(const unsigned char* data, unsigned long length) throw(IOException)
{
    if(data == NULL)
    {
        THROW_IOEXCEPTION("Can not update digest value from NULL pointer.");
    }

    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.");
    }

    if(SHA224_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA224 digest value: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Calculate message digest. SHA context will be invalid after this call.
 * For calculating an other digest you must create new SHA224Digest class.
 *
 * @return returns the calculated SHA224 digest.
 * @throws IOException throws exception if SHA224 update failed.
 */
std::vector<unsigned char> digidoc::SHA224Digest::getDigest() throw(IOException)
{
    // If digest is already calculated return it.
    if(!digest.empty())
    {
        return digest;
    }

    unsigned char buf[SHA224_DIGEST_LENGTH];
    if(SHA224_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA224 digest: %s", ERR_reason_error_string(ERR_get_error()));
    }

    for(unsigned int i = 0; i < SHA224_DIGEST_LENGTH; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}

/**
 * Initializes OpenSSL SHA256 digest calculator.
 *
 * @throws IOException throws exception if the SHA256 digest calculator initialization failed.
 */
digidoc::SHA256Digest::SHA256Digest() throw(IOException)
{
    method = NID_sha256;
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
    unsigned char digest[SHA256_DIGEST_LENGTH];
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
    if(data == NULL)
    {
        THROW_IOEXCEPTION("Can not update digest value from NULL pointer.");
    }

    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.");
    }

    if(SHA256_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA256 digest value: %s", ERR_reason_error_string(ERR_get_error()));
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

    unsigned char buf[SHA256_DIGEST_LENGTH];
    if(SHA256_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA256 digest: %s", ERR_reason_error_string(ERR_get_error()));
    }

    for(unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}

/**
 * Initializes OpenSSL SHA384 digest calculator.
 *
 * @throws IOException throws exception if the SHA384 digest calculator initialization failed.
 */
digidoc::SHA384Digest::SHA384Digest() throw(IOException)
{
    method = NID_sha384;
    if(SHA384_Init(&ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to initialize SHA384 digest calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Destroys OpenSSL SHA384 digest calculator.
 */
digidoc::SHA384Digest::~SHA384Digest()
{
    unsigned char digest[SHA384_DIGEST_LENGTH];
    if(SHA384_Final(digest, &ctx) != 1)
    {
        ERR("Failed to uninitialize SHA384 calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Add data for digest calculation. After calling <code>getDigest()</code> SHA384 context
 * is uninitialized and this method should not be called.
 *
 * @param data data to add for digest calculation.
 * @param length length of the data.
 * @throws IOException throws exception if SHA384 update failed.
 * @see getDigest()
 */
void digidoc::SHA384Digest::update(const unsigned char* data, unsigned long length) throw(IOException)
{
    if(data == NULL)
    {
        THROW_IOEXCEPTION("Can not update digest value from NULL pointer.");
    }

    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.");
    }

    if(SHA384_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA384 digest value: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Calculate message digest. SHA context will be invalid after this call.
 * For calculating an other digest you must create new SHA384Digest class.
 *
 * @return returns the calculated SHA384 digest.
 * @throws IOException throws exception if SHA384 update failed.
 */
std::vector<unsigned char> digidoc::SHA384Digest::getDigest() throw(IOException)
{
    // If digest is already calculated return it.
    if(!digest.empty())
    {
        return digest;
    }

    unsigned char buf[SHA384_DIGEST_LENGTH];
    if(SHA384_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA384 digest: %s", ERR_reason_error_string(ERR_get_error()));
    }

    for(unsigned int i = 0; i < SHA384_DIGEST_LENGTH; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}

/**
 * Initializes OpenSSL SHA512 digest calculator.
 *
 * @throws IOException throws exception if the SHA512 digest calculator initialization failed.
 */
digidoc::SHA512Digest::SHA512Digest() throw(IOException)
{
    method = NID_sha512;
    if(SHA512_Init(&ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to initialize SHA512 digest calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Destroys OpenSSL SHA512 digest calculator.
 */
digidoc::SHA512Digest::~SHA512Digest()
{
    unsigned char digest[SHA512_DIGEST_LENGTH];
    if(SHA512_Final(digest, &ctx) != 1)
    {
        ERR("Failed to uninitialize SHA512 calculator: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Add data for digest calculation. After calling <code>getDigest()</code> SHA512 context
 * is uninitialized and this method should not be called.
 *
 * @param data data to add for digest calculation.
 * @param length length of the data.
 * @throws IOException throws exception if SHA512 update failed.
 * @see getDigest()
 */
void digidoc::SHA512Digest::update(const unsigned char* data, unsigned long length) throw(IOException)
{
    if(data == NULL)
    {
        THROW_IOEXCEPTION("Can not update digest value from NULL pointer.");
    }

    if(!digest.empty())
    {
        THROW_IOEXCEPTION("Digest is already finalized, can not update it.");
    }

    if(SHA512_Update(&ctx, static_cast<const void*>(data), length) != 1)
    {
        THROW_IOEXCEPTION("Failed to update SHA512 digest value: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Calculate message digest. SHA context will be invalid after this call.
 * For calculating an other digest you must create new SHA512Digest class.
 *
 * @return returns the calculated SHA512 digest.
 * @throws IOException throws exception if SHA512 update failed.
 */
std::vector<unsigned char> digidoc::SHA512Digest::getDigest() throw(IOException)
{
    // If digest is already calculated return it.
    if(!digest.empty())
    {
        return digest;
    }

    unsigned char buf[SHA512_DIGEST_LENGTH];
    if(SHA512_Final(buf, &ctx) != 1)
    {
        THROW_IOEXCEPTION("Failed to create SHA512 digest: %s", ERR_reason_error_string(ERR_get_error()));
    }

    for(unsigned int i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        digest.push_back(buf[i]);
    }

    return digest;
}
