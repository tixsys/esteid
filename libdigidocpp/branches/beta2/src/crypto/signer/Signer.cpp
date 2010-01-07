#include "Signer.h"
#include "../../util/String.h"

/**
 * @param signatureProductionPlace place info, where the document is signed.
 */
void digidoc::Signer::setSignatureProductionPlace(const SignatureProductionPlace& signatureProductionPlace)
{
    this->signatureProductionPlace = signatureProductionPlace;
}


/**
 * @return returns signature production place info.
 */
digidoc::SignatureProductionPlace digidoc::Signer::getSignatureProductionPlace() const
{
    return signatureProductionPlace;
}

/**
 * @param signerRole role(s) of the signer.
 */
void digidoc::Signer::setSignerRole(const SignerRole& signerRole)
{
    this->signerRole = signerRole;
}

/**
 * @return returns signer role.
 */
digidoc::SignerRole digidoc::Signer::getSignerRole() const
{
    return signerRole;
}
