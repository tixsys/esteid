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

#include <openssl/pem.h>
#include <openssl/err.h>
#include "../../log.h"
#include "../../crypto/OpenSSLHelpers.h"
#include "../../crypto/cert/X509Cert.h"
#include "../../util/String.h"
#include "RSACrypt.h"

/**
 * Initialize RSA crypter.
 *
 * @param cert X.509 certificate.
 * @param privateKey RSA private key, must match with the provided X.509 certificate.
 */
digidoc::RSACrypt::RSACrypt(X509* cert, RSA* privateKey)
 : cert(cert)
 , privateKey(privateKey)
{
}

/**
 * Initialize RSA crypter.
 *
 * @param cert X.509 certificate.
 */
digidoc::RSACrypt::RSACrypt(X509* cert)
 : cert(cert)
 , privateKey(NULL)
{
}

/**
 * Initialize RSA crypter.
 *
 * @param privateKey RSA private key.
 */
digidoc::RSACrypt::RSACrypt(RSA* privateKey)
 : cert(NULL)
 , privateKey(privateKey)
{
}

/**
 *
 */
digidoc::RSACrypt::~RSACrypt()
{
}

/**
 * Signs the digest with provided RSA private key.
 *
 * @param digestMethod digest method (e.g NID_sha1 for SHA1, see openssl/obj_mac.h).
 * @param digest digest value, this value is signed with the private RSA key.
 * @return returns signature.
 * @throws IOException
 */
std::vector<unsigned char> digidoc::RSACrypt::sign(const Signer::Digest& digest) throw(IOException)
{
    // Calculate memory needed for signature.
    unsigned int blockSize = RSA_size(privateKey);
    unsigned int neededSize = blockSize;
    if(digest.length > blockSize)
    {
        if(digest.length % blockSize == 0)
            neededSize = digest.length;
        else
            neededSize = ((digest.length / blockSize) + 1) * blockSize;
    }

    // Allocate memory for the signature.
    std::vector<unsigned char> signature(neededSize, 0);

    // Sign the digest with private RSA key.
    unsigned int signatureLength = 0;
    int result = RSA_sign(digest.type, digest.digest, digest.length, &signature[0], &signatureLength, privateKey);

    // Check that signing was successful.
    if(result != 1)
    {
        THROW_IOEXCEPTION("Failed to sign the digest: %s", ERR_reason_error_string(ERR_get_error()));
    }

    if(signatureLength != neededSize)
    {
        THROW_IOEXCEPTION("Failed to sign the digest.");
    }

    return signature;
}

/**
 * Verify signature with RSA public key from X.509 certificate.
 *
 * @param digestMethod digest method (e.g NID_sha1 for SHA1, see openssl/obj_mac.h).
 * @param digest digest value, this value is compared with the digest value decrypted from the <code>signature</code>.
 * @param signature signature value, this value is decrypted to get the digest and compared with
 *        the digest value provided in <code>digest</code>.
 * @return returns <code>true</code> if the signature value matches with the digest, otherwise <code>false</code>
 *         is returned.
 * @throws IOException throws exception if X.509 certificate is not missing or does not have a RSA public key.
 */
bool digidoc::RSACrypt::verify(int digestMethod, std::vector<unsigned char> digest, std::vector<unsigned char> signature) throw(IOException)
{
    // Check that X.509 certificate is set.
    if(cert == NULL)
    {
        THROW_IOEXCEPTION("X.509 certificate parameter is not set in RSACrypt, can not verify signature.");
    }

    // Extract RSA public key from X.509 certificate.
    X509Cert x509(cert);
    EVP_PKEY* key = x509.getPublicKey();
    if(EVP_PKEY_type(key->type) != EVP_PKEY_RSA)
    {
        THROW_IOEXCEPTION("Certificate '%s' does not have a RSA public key, can not verify signature.", x509.getSubject().c_str());
    }
    RSA* publicKey = EVP_PKEY_get1_RSA(key);

    // Verify signature with RSA public key.
    int result = RSA_verify(digestMethod, &digest[0], digest.size(), &signature[0], signature.size(), publicKey);
    return (result == 1);
}

/**
 * Loads RSA private key from file.
 * NB! This struct must be freed using RSA_free() function from OpenSSL or
 * with RSA_scope struct.
 *
 * @param path PEM formatted RSA private key file path.
 * @return returns RSA private key parsed from file.
 * @throws IOException throws exception if the file does not contain RSA private key.
 */
RSA* digidoc::RSACrypt::loadRSAPrivateKey(const std::string& path) throw(IOException)
{
    // Initialize OpenSSL file.
    BIO* file = BIO_new(BIO_s_file()); BIO_scope fileScope(&file);
    if(file == NULL)
    {
        THROW_IOEXCEPTION("Failed to open RSA private key file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    if(BIO_read_filename(file, path.c_str()) <= 0)
    {
        THROW_IOEXCEPTION("Failed to open RSA private key file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    // Parse RSA private key from file.
    RSA* key = PEM_read_bio_RSAPrivateKey(file, NULL, NULL, NULL);
    if(key == NULL)
    {
        THROW_IOEXCEPTION("Failed to load RSA private key from file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    return key;
}
