#include <string.h>

#include <openssl/pem.h>
#include <openssl/err.h>

#include "../../log.h"
#include "../../crypto/OpenSSLHelpers.h"
#include "../../crypto/crypt/RSACrypt.h"
#include "../../io/IOException.h"
#include "../../util/String.h"
#include "RSASigner.h"


/**
 * Initializes the RSA signer with X.509 certificate and private key pair.
 *
 * @param cert X.509 certificate of the private key.
 * @param privateKey private key, should match the X.509 certificate.
 * @throws SignException throws exception if the certificate or the private key
 *         is NULL.
 */
digidoc::RSASigner::RSASigner(X509* cert, RSA* privateKey) throw(SignException)
 : cert(cert)
 , privateKey(privateKey)
{
    if(this->cert == NULL)
    {
        THROW_SIGNEXCEPTION("The provided X.509 certificate is NULL pointer.");
    }

    if(this->privateKey == NULL)
    {
        THROW_SIGNEXCEPTION("The provided private RSA key is NULL pointer.");
    }
}

/**
 *
 */
digidoc::RSASigner::~RSASigner()
{
}

/**
 * Returns the X.509 certificate used for signing.
 *
 * @return returns certificate used for signing.
 * @throws throws never thrown.
 */
X509* digidoc::RSASigner::getCert() throw(SignException)
{
    DEBUG("RSASigner::getCert()");
    return cert;
}

/**
 * Signs the provided digest using the private key that matches the X.509 certificate.
 *
 * @param digest digest, which is being signed.
 * @param signature memory for the signature that is created. Struct parameter <code>length</code>
 *        is set to the actual signature length.
 * @throws SignException throws exception if the signing operation failed or not enough memory
 *         allocated for the signature.
 */
void digidoc::RSASigner::sign(const Digest& digest, Signature& signature) throw(SignException)
{
    DEBUG("RSASigner::sign(digest = {type=%s,digest=0x%X,length=%d}, signature={signature=0x%X,length=%d})",
            OBJ_nid2sn(digest.type), (unsigned int)digest.digest, digest.length,
            (unsigned int)signature.signature, signature.length);

    try
    {
        // Sign the digest.
        RSACrypt rsa(privateKey);
        std::vector<unsigned char> sign = rsa.sign(digest);

        // Check that enough memory is allocated for the signature.
        if(sign.size() > signature.length)
        {
            THROW_SIGNEXCEPTION("Not enough memory for signature allocated, needs %d bytes, allocated %d bytes.",
        	        sign.size(), signature.length);
        }

        // Copy the signature to the buffer.
        memcpy(signature.signature, &sign[0], sign.size());
        signature.length = sign.size();
    }
    catch(const IOException& e)
    {
        THROW_SIGNEXCEPTION_CAUSE(e, "Failed to sign digest.");
    }
}
