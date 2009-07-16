#include "Signer.h"
#include "../../util/String.h"

/**
 * Initialized empty SignatureProductionPlace.
 */
digidoc::Signer::SignatureProductionPlace::SignatureProductionPlace()
{
}

/**
 * Helper constructor to initialize SignatureProductionPlace with values.
 *
 * @param city city, where document is signed.
 * @param stateOrProvince state or province, where document is signed.
 * @param postalCode postal code, where document is signed.
 * @param countryName country, where document is signed.
 */
digidoc::Signer::SignatureProductionPlace::SignatureProductionPlace(std::string city,
        std::string stateOrProvince, std::string postalCode, std::string countryName)
 : city(city)
 , stateOrProvince(stateOrProvince)
 , postalCode(postalCode)
 , countryName(countryName)
{
}

/**
 * @return returns whether the SignatureProductionPlace is initialized or not. If the method
 *         returns <code>true</code> the SignatureProductionPlace is not set.
 */
bool digidoc::Signer::SignatureProductionPlace::isEmpty()
{
    return city.empty() && stateOrProvince.empty() && postalCode.empty() && countryName.empty();
}


/**
 * Initialized empty SignatureProductionPlace.
 */
digidoc::Signer::SignerRole::SignerRole()
{
}

/**
 * Initializes SignerRole with one ClaimedRole item.
 *
 * @param claimedRole claimed role of the signer.
 */
digidoc::Signer::SignerRole::SignerRole(const std::string& claimedRole)
{
    claimedRoles.push_back(claimedRole);
}

/**
 * @return returns whether the SignerRole is initialized or not. If the method returns
 *         <code>true</code> the SignerRole is not set.
 */
bool digidoc::Signer::SignerRole::isEmpty()
{
    return claimedRoles.empty();
}

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
digidoc::Signer::SignatureProductionPlace digidoc::Signer::getSignatureProductionPlace() const
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
digidoc::Signer::SignerRole digidoc::Signer::getSignerRole() const
{
    return signerRole;
}
