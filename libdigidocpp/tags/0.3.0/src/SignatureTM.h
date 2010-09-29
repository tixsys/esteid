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

/*
 * SignatureTM.h
 *
 *  Created on: 26/01/2009
 *      Author: janno
 */

#ifndef SIGNATURETM_H_
#define SIGNATURETM_H_

#include "SignatureBES.h"


namespace digidoc
{

class EXP_DIGIDOC SignatureTM: public digidoc::SignatureBES
{
public:
    SignatureTM(BDoc& _bdoc);
    SignatureTM(const std::string& path, BDoc& _bdoc) throw(SignatureException);
    virtual ~SignatureTM();
    virtual std::string getMediaType() const;
    X509Cert getOCSPCertificate() const;
    std::string getProducedAt() const;
    std::string getResponderID() const;
    void getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri) const throw(SignatureException);
    virtual void validateOffline() const throw(SignatureException);

    static const std::string MEDIA_TYPE;
protected:
    virtual void sign(Signer* signer) throw(SignatureException, SignException);
    void addCertificateValue(const std::string& certId, const X509Cert& x509);
private:
    void createTMProperties();
    void setOCSPCertificate(const X509Cert& x509);
    void setCACertificate(const X509Cert& x509);
    void setOCSPResponseValue(const std::vector<unsigned char>& data);

    void setCompleteRevocationRefs(const std::string& responderName, const std::string& digestMethodUri,
            const std::vector<unsigned char>& ocspResponseHash, const struct tm& producedAt );

    void getOCSPResponseValue(std::vector<unsigned char>& data) const;
    xades::UnsignedPropertiesType::UnsignedSignaturePropertiesOptional& unsignedSignatureProperties() const;
};

}

#endif /* SIGNATURETM_H_ */
