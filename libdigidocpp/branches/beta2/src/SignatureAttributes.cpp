#include "SignatureAttributes.h"

/**
 * Initialized empty SignatureProductionPlace.
 */
digidoc::SignatureProductionPlace::SignatureProductionPlace()
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
digidoc::SignatureProductionPlace::SignatureProductionPlace(std::string city,
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
bool digidoc::SignatureProductionPlace::isEmpty()
{
    return city.empty() && stateOrProvince.empty() && postalCode.empty() && countryName.empty();
}


/**
 * Initialized empty SignerRole
 */
digidoc::SignerRole::SignerRole()
{
}

/**
 * Initializes SignerRole with one ClaimedRole item.
 *
 * @param claimedRole claimed role of the signer.
 */
digidoc::SignerRole::SignerRole(const std::string& claimedRole)
{
    claimedRoles.push_back(claimedRole);
}

/**
 * @return returns whether the SignerRole is initialized or not. If the method returns
 *         <code>true</code> the SignerRole is not set.
 */
bool digidoc::SignerRole::isEmpty()
{
    return claimedRoles.empty();
}
