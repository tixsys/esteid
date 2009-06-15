#include "SignatureBES.h"

/**
 * BES profile signature media type.
 */
const std::string digidoc::SignatureBES::MEDIA_TYPE = "signature/bdoc-1.0/BES";

/**
 *
 */
digidoc::SignatureBES::SignatureBES()
 : Signature()
{
}

/**
 *
 * @param path
 * @throws SignatureException
 */
digidoc::SignatureBES::SignatureBES(const std::string& path) throw(SignatureException)
 : Signature(path)
{
    checkIfWellFormed();
}

/**
 * @return returns signature mimetype.
 */
std::string digidoc::SignatureBES::getMediaType() const
{
    return MEDIA_TYPE;
}

void digidoc::SignatureBES::sign(Signer* signer)
{
    /*
    // TODO: add other fields to the signature
    X509* publicCert = signer->getCert();
    signature->setSigningCertificate(publicCert);

    signature->setSignatureProductionPlace(signer->getSignatureProductionPlace());
    signature->setSignerRole(signer->getSignerRole());

    // TODO: sign digest and add it to signature
    //digidoc::Signer::Digest digest = { NID_sha1, &docHash[0], digidoc::SHA1Digest::SHA1_DIGEST_SIZE };
    //digidoc::Signer::Signature signatureSha1Rsa = { new unsigned char[128], 128 };
    //signer->sign(digest, signatureSha1Rsa);

    //signature->addSignature(signatureSha1Rsa);

    */
}

/**
*
* @param path
* @return
*/
void digidoc::SignatureBES::checkIfWellFormed() throw(SignatureException)
{
    // Base class has verified the signature to be valid according to XML-DSig.
    // Now perform additional checks here and throw if this signature is
    // ill-formed according to BDoc.

    Signature::getSignedSignatureProperties(); // assumed to throw in case this block doesn't exist

    // TODO: implement more checks?
}

