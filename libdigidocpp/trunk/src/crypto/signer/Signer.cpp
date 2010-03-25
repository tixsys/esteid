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
