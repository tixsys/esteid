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

#include <string.h>

#include "../../log.h"

#include "WebSigner.h"

/**
 * Initializes Web signer with X.509 certificate.
 *
 * @param cert X.509 certificate
 * @throws SignException throws exception if the certificate is NULL.
 */

digidoc::WebSigner::WebSigner(const X509Cert& cert) throw(SignException)
{
    this->cert = cert.getX509();

    if(this->cert == NULL)
    {
        THROW_SIGNEXCEPTION("The provided X.509 certificate is NULL pointer.");
    }
}

/**
 *
 */
digidoc::WebSigner::~WebSigner()
{
}

/**
 * Returns the X.509 certificate used for signing.
 *
 * @return returns certificate used for signing.
 * @throws SignException for this class, it's never thrown.
 */
X509* digidoc::WebSigner::getCert() throw(SignException)
{
    DEBUG("WebSigner::getCert()");
    return cert;
}

/**
 * Throws signing exception. Web signing is a two-part
 * async operation so this is here to guard against
 * other classes from accidentally using the same object
 * for both operations.
 *
 * @throws SignException this function always throws 
 */
void digidoc::WebSigner::sign(const Digest& digest, Signature& signature) throw(SignException)
{
    THROW_SIGNEXCEPTION("WebSigner can only be used for preparing the signature");
}


/**
 * Initializes Web signature from SignatureValue object
 *
 * @param sig signature value
 * @throws SignException never throws
 */
digidoc::WebSignature::WebSignature(const SignatureValue& sig) throw(SignException)
{
    // TODO: actually use SignatureType.method
    sigVal = sig;
}

/**
 * Initializes Web signature from signature Hex
 *
 * @param sigHex hex encoded signature value
 * @throws SignException throws exception if the value is too short
 */
digidoc::WebSignature::WebSignature(const std::string& sig) throw(SignException)
{
    if(sig.size() < 256) {
        THROW_SIGNEXCEPTION("Passed string is too short");
    }

    sigVal = SignatureValue(sig);
}

/**
 *
 */
digidoc::WebSignature::~WebSignature()
{
}

/**
 * Throws signing exception. Web signing is a two-part
 * async operation so this is here to guard against
 * other classes from accidentally using the same object
 * for both operations.
 *
 * @throws SignException this function always throws 
 */
X509* digidoc::WebSignature::getCert() throw(SignException)
{
    THROW_SIGNEXCEPTION("WebSignature can only be used for finalizing the signature");
    return NULL; // Dummy return to make OpenSuse compilers happy
}

/**
 * Return sigHex as a result of the two-part signing operation
 *
 * @param digest digest, which is being signed.
 * @param signature memory for the signature that is created. Struct parameter <code>length</code>
 *        is set to the actual signature length.
 * @throws SignException throws exception if not enough memory allocated for the signature.
 */
void digidoc::WebSignature::sign(const Digest& digest, Signature& signature) throw(SignException)
{
    DEBUG("WebSigner::sign(digest = {type=%s,digest=0x%p,length=%d}, signature={signature=0x%p,length=%d})",
            OBJ_nid2sn(digest.type), digest.digest, digest.length,
            signature.signature, signature.length);

    // Check that enough memory is allocated for the signature.
    if(sigVal.value.size() > signature.length)
    {
        THROW_SIGNEXCEPTION("Not enough memory for signature allocated, needs %d bytes, allocated %d bytes.",
            sigVal.value.size(), signature.length);
    }

    // Copy the signature to the buffer.
    memcpy(signature.signature, &sigVal.value[0], sigVal.value.size());
    signature.length = sigVal.value.size();
}
