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
