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

class SignatureTM: public digidoc::SignatureBES
{
public:
    SignatureTM(BDoc& _bdoc);
    SignatureTM(const std::string& path, BDoc& _bdoc) throw(SignatureException);
    virtual ~SignatureTM();
    virtual std::string getMediaType() const;
    virtual void validateOffline() const throw(SignatureException);

    void getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri) const throw(SignatureException);

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
};

}

#endif /* SIGNATURETM_H_ */
