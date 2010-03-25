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

#if !defined(__RSACRYPT_H_INCLUDED__)
#define __RSACRYPT_H_INCLUDED__

#include <openssl/x509.h>

#include <string>
#include <vector>

#include "../../crypto/signer/Signer.h"
#include "../../io/IOException.h"

namespace digidoc
{
    /**
     * Wrapper for RSA crypto function in OpenSSL. Currently implemented RSA signature
     * verification and signing with private RSA key.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC RSACrypt
    {

      public:
          RSACrypt(X509* cert, RSA* privateKey);
          RSACrypt(X509* cert);
          RSACrypt(RSA* privateKey);
          ~RSACrypt();
          std::vector<unsigned char> sign(const Signer::Digest& digest) throw(IOException);
          bool verify(int digestMethod, std::vector<unsigned char> digest, std::vector<unsigned char> signature) throw(IOException);
          static RSA* loadRSAPrivateKey(const std::string& path) throw(IOException);

      private:
          X509* cert;
          RSA* privateKey;

    };
}

#endif // !defined(__RSACRYPT_H_INCLUDED__)
