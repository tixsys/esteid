/*
 * SignatureTM.cpp
 */

#include "SignatureTM.h"
#include "Conf.h"
#include "crypto/Digest.h"
#include "crypto/cert/X509Cert.h"
#include "crypto/cert/X509CertStore.h"
#include "crypto/cert/DirectoryX509CertStore.h"
#include "crypto/ocsp/OCSP.h"
#include "util/DateTime.h"
#include "log.h"

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>


#include <openssl/ssl.h>
#include <openssl/conf.h>


/**
 * TM profile signature media type.
 */
const std::string digidoc::SignatureTM::MEDIA_TYPE = "signature/bdoc-1.0/TM";

digidoc::SignatureTM::SignatureTM(BDoc& _bdoc)
: SignatureBES(_bdoc)
{
}

digidoc::SignatureTM::SignatureTM(const std::string& path, BDoc& _bdoc) throw(SignatureException)
: SignatureBES(path, _bdoc)
{
}

digidoc::SignatureTM::~SignatureTM()
{
}

/**
 * @return returns signature mimetype.
 */
std::string digidoc::SignatureTM::getMediaType() const
{
    return MEDIA_TYPE;
}

/**
 * Do TM offline validations.
 * <ul>
 *   <li>Validate BES offline</li>
 *   li>Check OCSP response (RevocationValues) was signed by trusted OCSP server</li>
 *   <li>Check that nonce field in OCSP response is same as CompleteRevocationRefs-&gt;DigestValue</li>
 *   <li>Recalculate hash of signature and compare with nonce</li>
 * </ul>
 * @throws SignatureException if signature is not valid
 */
void digidoc::SignatureTM::validateOffline() const throw(SignatureException)
{
    SignatureBES::validateOffline();

//       1. Check OCSP response (RevocationValues) was signed by OCSP server
//       2. OCSP server certificate is trusted?
//       3. Check that nonce field in OCSP response is same as CompleteRevocationRefs->DigestValue
//       4. Recalculate hash of signature and compare with nonce

    OCSP ocsp(Conf::getInstance()->getOCSPUrl());
    STACK_OF(X509)* ocspCerts = X509Cert::loadX509Stack(Conf::getInstance()->getOCSPCertPath());
    X509Stack_scope x509StackScope(&ocspCerts);
    ocsp.setOCSPCerts(ocspCerts);
    //ocsp.setCertStore(digidoc::X509CertStore::getInstance()->getCertStore());
    std::vector<unsigned char> respBuf;
    getOCSPResponseValue(respBuf);
    ocsp.verifyResponse(respBuf);
    DEBUG("OCSP response was signed by trusted OCSP responder");

    std::vector<unsigned char> respNonce = ocsp.getNonce(respBuf);
    dsig::SignatureValueType sig = signature->signatureValue();
    const unsigned char* signatureBuf = reinterpret_cast<const unsigned char*>( sig.data() );
    // FIXME: Take digest URI from XML
    std::auto_ptr<Digest> digest = Digest::create(std::string("http://www.w3.org/2000/09/xmldsig#sha1"));
    digest->update(&signatureBuf[0], sig.size());
    std::vector<unsigned char> signatureHash = digest->getDigest();


    std::vector<unsigned char> revocationNonce(0);
    getRevocationNonce(revocationNonce);

    DEBUGMEM("Calculated signature HASH", &signatureHash[0], signatureHash.size());
    DEBUGMEM("Response nonce", &respNonce[0], respNonce.size());
    DEBUGMEM("Revocation nonce", &revocationNonce[0], revocationNonce.size());

    if((signatureHash != respNonce)
        || (signatureHash != revocationNonce))
    {
        //FIXME: improve message
        THROW_SIGNATUREEXCEPTION("Calculated signature hash doesn't match to OCSP responder nonce field");
    }
    else
    {
        DEBUG("TM signature valid");
    }



}

void digidoc::SignatureTM::sign(Signer* signer) throw(SignatureException)
{
    DEBUG("digidoc::SignatureTM::sign()");
    SignatureBES::sign(signer);
    DEBUG("BES successful");

    dsig::SignatureValueType sig = signature->signatureValue();


    const unsigned char* signatureBuf = reinterpret_cast<const unsigned char*>( sig.data() );

    // FIXME: Use Digest::create() to get configured digest calculator
    std::auto_ptr<Digest> digest = Digest::create(std::string("http://www.w3.org/2000/09/xmldsig#sha1"));
    digest->update(&signatureBuf[0], sig.size());
    std::vector<unsigned char> nonce = digest->getDigest();

    //DEBUGMEM("signatureBuf", &signatureBuf[0], sig.size());
    DEBUGMEM("Calculated signature HASH (nonce):", &nonce[0], nonce.size());

    //FIXME:: read from digidoc instead
    // NOTE: See ei ole tegelt probleem, kuna valitud cert cachetakse ja tagastatakse sama cert, mis eelmine kord anti.
    //signer->getCert() is owned by signer, dont delete
    X509 * signingX509 = signer->getCert();


    X509Cert signingCert(signingX509);

    DEBUG("Signing X509Cert{%ld,%s})",  signingCert.getSerial(), signingCert.getSubject().c_str());
    DEBUG("Issuer name %s",  signingCert.getIssuerName().c_str());
    X509CertStore* certStore = digidoc::X509CertStore::getInstance();
    X509* issuerX509 = certStore->getCert(*(signingCert.getIssuerNameAsn1()));
    X509_scope tempIssuerScope(&issuerX509);


    //make OCSP request
    std::vector<unsigned char> ocspResponse;
    struct tm producedAt;

    DEBUG("Making OCSP request");
    OCSP ocsp(Conf::getInstance()->getOCSPUrl());
    STACK_OF(X509)* ocspCerts =
         X509Cert::loadX509Stack(Conf::getInstance()->getOCSPCertPath());
    X509Stack_scope x509StackScope(&ocspCerts);
    //ocsp.setCertStore(digidoc::X509CertStore::getInstance()->getCertStore());
    ocsp.setMaxAge(5);
    ocsp.setSkew(120);
    digidoc::OCSP::CertStatus status = ocsp.checkCert(signingX509,
            issuerX509, nonce, ocspResponse, producedAt);

    if(status == digidoc::OCSP::GOOD) { DEBUG("OCSP status: GOOD"); }
    else if(status == digidoc::OCSP::REVOKED) { DEBUG("OCSP status: REVOKED"); }
    else if(status == digidoc::OCSP::UNKNOWN) { DEBUG("OCSP status: UNKNOWN"); }

    DEBUG("OCSP response size %d", ocspResponse.size());

    //FIXME: get from ocsp instead
    // FIXME: This file can contain multiple certs. X509Cert class supports only one cert per file
    // loadX509Stack loads multiple certs from one file
    X509* ocspX509 = X509Cert::loadX509(Conf::getInstance()->getOCSPCertPath());
    X509Cert responderCert(ocspX509);
    X509_scope t(&ocspX509);

    X509Cert issuerCert(issuerX509);
    DEBUG("Issuer X509Cert{%ld,%s})",  issuerCert.getSerial(), issuerCert.getSubject().c_str());

    createTMProperties();
    setOCSPCertificate(responderCert);
    setCACertificate(issuerCert);

    setCompleteRevocationRefs(responderCert.getIssuerName(), nonce, producedAt);
    setOCSPResponseValue(ocspResponse);

}

/**
 * Add elements required by TM profile
 */
void digidoc::SignatureTM::createTMProperties()
{
    xades::QualifyingPropertiesType qp = signature->object()[0].qualifyingProperties()[0];

    xades::UnsignedPropertiesType usProp;

    xades::UnsignedSignaturePropertiesType usSignatureProp;
    usProp.unsignedSignatureProperties(usSignatureProp);


    signature->object()[0].qualifyingProperties()[0].unsignedProperties(usProp);

}


/**
 * Create CompleteRevocationRefs element that describes communication with OSCP responder.
 * @param responderName OCSP responder name as represented in responder public certification. Format as RFC2253
 * @param nonce Nonce field of OCSP request. Hash of signature
 * @param producedAt ProduceAt field of OCSP response
 */
void digidoc::SignatureTM::setCompleteRevocationRefs(const std::string& responderName,
        const std::vector<unsigned char>& nonce, const struct tm& producedAt )
{
    dsig::DigestMethodType method(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1"));
    dsig::DigestValueType value(xml_schema::Base64Binary(&nonce[0], nonce.size()));

    xades::DigestAlgAndValueType digest(method, value);


    xades::ResponderIDType responderId;
    responderId.byName(xml_schema::String(responderName.c_str()));

    xml_schema::DateTime dateTime(
            producedAt.tm_year + 1900,
            producedAt.tm_mon + 1,
            producedAt.tm_mday,
            producedAt.tm_hour,
            producedAt.tm_min,
            producedAt.tm_sec, 0,0);
    xades::OCSPIdentifierType ocspId(responderId, dateTime);
    ocspId.uRI(xml_schema::Uri("#N0"));

    xades::OCSPRefType ocspRef(ocspId);
    ocspRef.digestAlgAndValue(digest);


    xades::OCSPRefsType ocspRefs;
    ocspRefs.oCSPRef().push_back(ocspRef);

    xades::CompleteRevocationRefsType completeRevocationRefs;
    completeRevocationRefs.oCSPRefs(ocspRefs);
    completeRevocationRefs.id(xml_schema::Id("S0-REVOCREFS"));
    //return completeRevocationRefs;

    signature->object()[0].qualifyingProperties()[0].unsignedProperties()->unsignedSignatureProperties()
        ->completeRevocationRefs().push_back(completeRevocationRefs);
}

/**
 * Set OCSP responder cert.
 * @param x509 certificate that was used identify OCSP responder
 */
void digidoc::SignatureTM::setOCSPCertificate(const X509Cert& x509)
{
    //XXX: copied from digidoc::Signature::setSigningCertificate

    DEBUG("digidoc::SignatureTM::setOCSPCertificate()");

    std::vector<unsigned char> derEncodedX509 = x509.encodeDER();

    // Calculate SHA1 digest of the certificate.
    SHA1Digest certSha1;
    certSha1.update(&derEncodedX509[0], derEncodedX509.size());
    dsig::DigestMethodType digestMethod(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1"));
    dsig::DigestValueType digestValue(xml_schema::Base64Binary(&certSha1.getDigest()[0], SHA1Digest::DIGEST_SIZE));
    xades::DigestAlgAndValueType certDigest(digestMethod, digestValue);

    // Add certificate issuer info.
    dsig::X509IssuerSerialType issuerSerial(xml_schema::String(x509.getIssuerName()), xml_schema::Integer(x509.getSerial()));

    digidoc::xades::CertIDType cert(certDigest, issuerSerial);
    xades::CertIDListType certList;
    certList.cert().push_back(cert);

    xades::CompleteCertificateRefsType certificateRefs(certList);

    //certificateRefs.certRefs(certList);

    signature->object()[0].qualifyingProperties()[0].unsignedProperties()->unsignedSignatureProperties()
        ->completeCertificateRefs().push_back(certificateRefs);


    //CertificateValues
    setCertificateValue(std::string("S0-RESPONDER_CERT"), x509);
}

void digidoc::SignatureTM::setCACertificate(const X509Cert& x509)
{
    setCertificateValue(std::string("S0-CA-CERT"), x509);
}

/**
 * Add certificate under CertificateValues element
 * @param certId id attribute of EncapsulatedX509Certificate
 * @param x509 value of EncapsulatedX509Certificate
 */
void digidoc::SignatureTM::setCertificateValue(const std::string& certId, const X509Cert& x509)
{
    DEBUG("digidoc::SignatureTM::setCertificateValue(%s, X509Cert{%ld,%s})", certId.c_str(), x509.getSerial(), x509.getSubject().c_str());
    //CertificateValues
    std::vector<unsigned char> certBytes = x509.encodeDER();
    xades::CertificateValuesType::EncapsulatedX509CertificateType certData(
        xml_schema::Base64Binary(&certBytes[0], certBytes.size()));

    xades::CertificateValuesType certValue;
    certValue.id(xml_schema::Id(certId.c_str()));
    certValue.encapsulatedX509Certificate().push_back(certData);

    signature->object()[0].qualifyingProperties()[0].unsignedProperties()->unsignedSignatureProperties()
        ->certificateValues().push_back(certValue);

}

/**
 * Fill  RevocationValues element
 * @param data OCSP response in DER encoding
 */
void digidoc::SignatureTM::setOCSPResponseValue(const std::vector<unsigned char>& data)
{
    xades::RevocationValuesType revocationValues;
    xades::OCSPValuesType ocspValue;
    xades::OCSPValuesType::EncapsulatedOCSPValueType ocspValueData(
        xml_schema::Base64Binary(&data[0], data.size()));

    ocspValueData.id(xml_schema::Id("N0"));
    ocspValue.encapsulatedOCSPValue().push_back(ocspValueData);
    revocationValues.oCSPValues(ocspValue);

    signature->object()[0].qualifyingProperties()[0].unsignedProperties()->unsignedSignatureProperties()
        ->revocationValues().push_back(revocationValues);
}
/**
 * Get value of UnsignedProperties\UnsignedSignatureProperties\RevocationValues\OCSPValues\EncapsulatedOCSPValue
 * which contains whole OCSP response
 * @param data DER encoded OCSP response
 */
void digidoc::SignatureTM::getOCSPResponseValue(std::vector<unsigned char>& data) const
{
    //FIXME: check that all elements exist
    xades::RevocationValuesType t = signature->object()[0].qualifyingProperties()[0]
        .unsignedProperties()->unsignedSignatureProperties()->revocationValues()[0];

    xades::OCSPValuesType tt = t.oCSPValues().get();
    xades::OCSPValuesType::EncapsulatedOCSPValueType resp = tt.encapsulatedOCSPValue()[0];

    data.resize(resp.size());
    std::copy(resp.data(), resp.data()+resp.size(), data.begin());

}

/**
 * Get value of UnsignedProperties\UnsignedSignatureProperties\CompleteRevocationRefs\OCSPRefs\OCSPRef\DigestAlgAndValue\DigestValue
 * which contains nonce that was sent OCSP server.
 * @param data DER encoded OCSP response
 * @throws SignatureException on error in references
 */
void digidoc::SignatureTM::getRevocationNonce(std::vector<unsigned char>& nonce)
    const throw(SignatureException)
{
    xades::UnsignedSignaturePropertiesType::CompleteRevocationRefsSequence crrSeq =
        signature->object()[0].qualifyingProperties()[0]
            .unsignedProperties()->unsignedSignatureProperties()->completeRevocationRefs();

    if(!crrSeq.empty())
    {
        xades::CompleteRevocationRefsType::OCSPRefsOptional ocspRefsOpt = crrSeq[0].oCSPRefs();
        if(ocspRefsOpt.present())
        {
            xades::OCSPRefsType::OCSPRefSequence ocspRefSeq = ocspRefsOpt->oCSPRef();
            if(!ocspRefSeq.empty())
            {
                xades::OCSPRefType::DigestAlgAndValueOptional digestOpt = ocspRefSeq[0].digestAlgAndValue();
                if(digestOpt.present())
                {
                    dsig::DigestValueType digestValue = digestOpt->digestValue();
                    nonce.resize(digestValue.size());
                    std::copy(digestValue.data(), digestValue.data()+digestValue.size(), nonce.begin());

                    return;
                }
            }
        }
    }


    //XXX: improve exception message
    THROW_SIGNATUREEXCEPTION("Missing UnsignedProperties/UnsignedSignatureProperties/CompleteRevocationRefs/OCSPRefs/OCSPRef/DigestAlgAndValue/DigestValue element or its parent");
}


