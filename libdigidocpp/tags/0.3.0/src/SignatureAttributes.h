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

#if !defined(__SIGNATUREATTRIBUTES_H_INCLUDED__)
#define __SIGNATUREATTRIBUTES_H_INCLUDED__

#include "Exports.h"

#include <string>
#include <vector>

namespace digidoc {

    class EXP_DIGIDOC SignatureProductionPlace
    {
    public:
        SignatureProductionPlace();
        SignatureProductionPlace(std::string city, std::string stateOrProvince, std::string postalCode, std::string countryName);
        bool isEmpty();

        std::string city;
        std::string stateOrProvince;
        std::string postalCode;
        std::string countryName;
    };

    class EXP_DIGIDOC SignerRole
    {
    public:
        SignerRole();
        SignerRole(const std::string& claimedRole);
        bool isEmpty();

        typedef std::vector<std::string> TRoles;
//        TRoles claimedRoles;
        std::vector<std::string> claimedRoles;
    };
}

#endif // !defined(__SIGNATUREATTRIBUTES_H_INCLUDED__)
