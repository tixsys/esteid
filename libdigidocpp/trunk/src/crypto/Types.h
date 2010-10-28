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

#if !defined(__CRYPTOTYPES_H_INCLUDED__)
#define __CRYPTOTYPES_H_INCLUDED__

#include "../Exports.h"
#include <vector>
#include <string>
#include <openssl/objects.h>


namespace digidoc
{
    /**
     * Wraps a final calculated digest
     */
    class EXP_DIGIDOC DigestValue
    {
      public:
        /** Digest type (e.g NID_sha1), available values in openssl/obj_mac.h */
        int type;
        std::vector <unsigned char> value;
        std::string toHexString();
    };

    /**
     * Wraps a calculated signature
     *
     * TODO: Use it more!
     */
    class EXP_DIGIDOC SignatureValue
    {
      public:
        SignatureValue() { };
        SignatureValue(const std::string& sigHex);
         /** Signature method type (e.g NID_sha1WithRSAEncryption), 
           * available values in openssl/obj_mac.h */
        int method;
        std::vector <unsigned char> value;
    };
}
#endif // !defined(__CRYPTOTYPES_H_INCLUDED__)
