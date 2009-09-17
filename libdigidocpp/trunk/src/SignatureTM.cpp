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
 *   <li>Check OCSP response (RevocationValues) was signed by trusted OCSP server</li>
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

    Conf* conf = Conf::getInstance();
    Conf::OCSPConf ocspConf = conf->getOCSP(getSigningCertificate().getIssuerName());
    if(ocspConf.issuer.empty())
    {
       THROW_SIGNATUREEXCEPTION("Failed to load ocsp issuer certificate.");
    }
    OCSP ocsp(ocspConf.url, conf->getProxyHost(), conf->getProxyPort());
    STACK_OF(X509)* ocspCerts = X509Cert::loadX509Stack(ocspConf.cert);
    X509Stack_scope x509StackScope(&ocspCerts);
    ocsp.setOCSPCerts(ocspCerts);
    //ocsp.setCertStore(digidoc::X509CertStore::getInstance()->getCertStore());
    std::vector<unsigned char> respBuf;
    getOCSPResponseValue(respBuf);
    try
    {
        ocsp.verifyResponse(respBuf);
    }
    catch( const Exception &e )
    {
        THROW_SIGNATUREEXCEPTION_CAUSE( e, "OCSP response verfiy failed" );
    }
    DEBUG("OCSP response was signed by trusted OCSP responder");



    std::vector<unsigned char> respNonce = ocsp.getNonce(respBuf);

    xml_schema::Uri method = signature->object()[0].qualifyingProperties()[0].unsignedProperties()
            ->unsignedSignatureProperties()->completeRevocationRefs()[0].oCSPRefs()
            ->oCSPRef()[0].digestAlgAndValue()->digestMethod().algorithm();

    std::auto_ptr<Digest> calc = Digest::create(std::string(method));
    calc->update(getSignatureValue());
    std::vector<unsigned char> nonce = calc->getDigest();

    if(nonce != respNonce)
    {
        DEBUGMEM("Calculated signature HASH", &nonce[0], nonce.size());
        DEBUGMEM("Response nonce", &respNonce[0], respNonce.size());
        THROW_SIGNATUREEXCEPTION("Calculated signature hash doesn't match to OCSP responder nonce field");
    }



    std::vector<unsigned char> revocationOCSPRefValue(0);
    std::string ocspResponseHashUri;
    getRevocationOCSPRef(revocationOCSPRefValue, ocspResponseHashUri);

    std::auto_ptr<Digest> ocspResponseCalc = Digest::create(ocspResponseHashUri);
    DEBUG("Calculating digest on %d bytes", respBuf.size());
    ocspResponseCalc->update(respBuf);
    std::vector<unsigned char> ocspResponseHash = ocspResponseCalc->getDigest();

    if(ocspResponseHash != revocationOCSPRefValue)
    {
        DEBUGMEM("Document   ocspResponse HASH:", &revocationOCSPRefValue[0], revocationOCSPRefValue.size());
        DEBUGMEM("Calculated ocspResponse HASH:", &ocspResponseHash[0], ocspResponseHash.size());
        THROW_SIGNATUREEXCEPTION("OCSPRef value doesn't match with hash of OCSP response");
    }
    else
    {
        DEBUG("TM signature valid");
    }

}

/**
 *
 * @param signer
 * @throws SignatureException
 */
void digidoc::SignatureTM::sign(Signer* signer) throw(SignatureException, SignException)
{
    DEBUG("SignatureTM::sign()");

    // Sign with BES profile.
    SignatureBES::sign(signer);
    DEBUG("BES signature successful.");

    // Calculate NONCE value.
    std::auto_ptr<Digest> calc = Digest::create();
    calc->update(getSignatureValue());
    std::vector<unsigned char> nonce = calc->getDigest();
    DEBUGMEM("Calculated signature HASH (nonce):", &nonce[0], nonce.size());

    // Get issuer certificate from certificate store.
    X509* cert = signer->getCert();
    X509Cert cert_(cert);
    X509* issuer = X509CertStore::getInstance()->getCert(*(cert_.getIssuerNameAsn1())); X509_scope issuerScope(&issuer);
    if(issuer == NULL)
    {
    	THROW_SIGNATUREEXCEPTION("Could not find certificate '%s' issuer '%s' in certificate store.",
    			cert_.getSubject().c_str(), cert_.getIssuerName().c_str());
    }

    DEBUG("Signing with X.509 cert {serial=%ld, subject=%s, issuer=%s})",
    		cert_.getSerial(), cert_.getSubject().c_str(), cert_.getIssuerName().c_str());

    // Initialize OCSP.
    DEBUG("Making OCSP request.");
    Conf* conf = Conf::getInstance();
    Conf::OCSPConf ocspConf = conf->getOCSP(cert_.getIssuerName());
    if(ocspConf.issuer.empty())
    {
        THROW_SIGNEXCEPTION("Failed to load ocsp issuer certificate.");
    }
    OCSP ocsp(ocspConf.url, conf->getProxyHost(), conf->getProxyPort());
    STACK_OF(X509)* ocspCerts = X509Cert::loadX509Stack(ocspConf.cert);
    X509Stack_scope ocspCertsScope(&ocspCerts);
    ocsp.setOCSPCerts(ocspCerts);
    ocsp.setMaxAge(5); // FIXME: remove or move to conf
    ocsp.setSkew(120); // FIXME: remove or move to conf

    // Make OCSP request.
    std::vector<unsigned char> ocspResponse;
    struct tm producedAt;
    OCSP::CertStatus status;
    try
    {
        status = ocsp.checkCert(cert, issuer, nonce, ocspResponse, producedAt);
    }
    catch(const IOException& e)
    {
        THROW_SIGNATUREEXCEPTION_CAUSE(e, "Failed to get OCSP response");
    }
    catch(const OCSPException& e)
    {
        THROW_SIGNATUREEXCEPTION_CAUSE(e, "Failed to get OCSP response");
    }

    switch(status)
    {
    case digidoc::OCSP::GOOD: DEBUG("OCSP status: GOOD"); break;
    case digidoc::OCSP::REVOKED:
    {
        DEBUG("OCSP status: REVOKED");
        SignatureException e( __FILE__, __LINE__, "Certificate status: revoked" );
        e.setCode( Exception::CertificateRevoked );
        throw e;
        break;
    }
    case digidoc::OCSP::UNKNOWN:
    {
        DEBUG("OCSP status: UNKNOWN");
        SignatureException e( __FILE__, __LINE__, "Certificate status: unknown" );
        e.setCode( Exception::CertificateUnknown );
        throw e;
        break;
    }
    }
    DEBUG("OCSP response size %d", ocspResponse.size());

    // FIXME: get from ocsp instead
    // FIXME: This file can contain multiple certs. X509Cert class supports only one cert per file
    // loadX509Stack loads multiple certs from one file
    // X509* ocspCert = X509Cert::loadX509(Conf::getInstance()->getOCSPCertPath()); X509_scope ocspCertScope(&ocspCert);
    if(sk_X509_num(ocspCerts) > 1)
    {
        ERR("More than one OCSP cert in file.");
    }
    X509Cert ocspCert_(sk_X509_value(ocspCerts, 0));

    std::auto_ptr<Digest> ocspResponseCalc = Digest::create();
    ocspResponseCalc->update(ocspResponse);
    std::vector<unsigned char> ocspResponseHash = ocspResponseCalc->getDigest();
    DEBUGMEM("Calculated ocspResponse HASH:", &ocspResponseHash[0], ocspResponseHash.size());

    // Set TM profile signature parameters.
    createTMProperties();
    setOCSPCertificate(ocspCert_);
    setCACertificate(X509Cert(issuer));
    setCompleteRevocationRefs(ocspCert_.getIssuerName(), calc->getUri(), ocspResponseHash, producedAt);
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
 * @param digestMethodUri digest method URI that was used for calculating ocspResponseHash
 * @param ocspResponseHash Digest of DER encode OCSP response
 * @param producedAt ProduceAt field of OCSP response
 */
void digidoc::SignatureTM::setCompleteRevocationRefs(const std::string& responderName, const std::string& digestMethodUri,
        const std::vector<unsigned char>& ocspResponseHash, const struct tm& producedAt )
{
    dsig::DigestMethodType method(xml_schema::Uri(digestMethodUri.c_str()));
    dsig::DigestValueType value(xml_schema::Base64Binary(&ocspResponseHash[0], ocspResponseHash.size()));

    xades::DigestAlgAndValueType digest(method, value);

    xades::ResponderIDType responderId;
    responderId.byName(xml_schema::String(responderName.c_str()));

	xml_schema::DateTime dateTime( util::date::makeDateTime( producedAt ) );
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
    std::auto_ptr<Digest> calc = Digest::create();
    calc->update(derEncodedX509);
    dsig::DigestMethodType digestMethod(xml_schema::Uri(calc->getUri()));
    dsig::DigestValueType digestValue(xml_schema::Base64Binary(&calc->getDigest()[0], calc->getSize()));
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
    addCertificateValue(std::string("S0-RESPONDER_CERT"), x509);
}

void digidoc::SignatureTM::setCACertificate(const X509Cert& x509)
{
    addCertificateValue(std::string("S0-CA-CERT"), x509);
}

/**
 * Add certificate under CertificateValues element
 * @param certId id attribute of EncapsulatedX509Certificate
 * @param x509 value of EncapsulatedX509Certificate
 */
void digidoc::SignatureTM::addCertificateValue(const std::string& certId, const X509Cert& x509)
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
 * @param data will contain DER encoded OCSP response bytes
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

    //DEBUG("EncapsulatedOCSPValue BASE64:\n %s\n", resp.encode().c_str());

    //DEBUGMEM("EncapsulatedOCSPValue DER:", resp.data(), resp.size());
    //DEBUGMEM("EncapsulatedOCSPValue DER:", &data[0], data.size());
}

/**
 * Get value of UnsignedProperties\UnsignedSignatureProperties\CompleteRevocationRefs\OCSPRefs\OCSPRef\DigestAlgAndValue
 * which contains hash of OCSP server response.
 * @param data Binary value of UnsignedProperties\UnsignedSignatureProperties\CompleteRevocationRefs\OCSPRefs\OCSPRef\DigestAlgAndValue\DigestValue. Hash of DER encoded OCSP response
 * @param digestMethodUri Digest method used for calculating hash of OCSP response
 * @throws SignatureException on error in references
 */
void digidoc::SignatureTM::getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri)
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
                    data.resize(digestValue.size());
                    std::copy(digestValue.data(), digestValue.data()+digestValue.size(), data.begin());

                    xml_schema::Uri uri = digestOpt->digestMethod().algorithm();
                    digestMethodUri = uri;

                    return;
                }
            }
        }
    }


    //XXX: improve exception message
    THROW_SIGNATUREEXCEPTION("Missing UnsignedProperties/UnsignedSignatureProperties/CompleteRevocationRefs/OCSPRefs/OCSPRef/DigestAlgAndValue element or its parent");
}


