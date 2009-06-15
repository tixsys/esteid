#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include <openssl/objects.h>
#include <openssl/err.h>



#include "log.h"
#include "Conf.h"
#include "Signature.h"
#include "util/File.h"
#include "util/String.h"
#include "util/DateTime.h"
#include "xml/OpenDocument_manifest.hxx"
#include "xml/xmldsig-core-schema.hxx"
#include "xml/XAdES.hxx"

const std::string digidoc::Signature::XADES_NAMESPACE("http://uri.etsi.org/01903/v1.3.2#");
const std::string digidoc::Signature::DSIG_NAMESPACE("http://www.w3.org/2000/09/xmldsig#");

/**
 *
 */
digidoc::Signature::Signature()
 : signature(NULL)
 , path("")
{
	signature = createEmptySignature();
}

/**
 *
 * @param path
 */
digidoc::Signature::Signature(const std::string& path)
 : signature(NULL)
 , path(path)
{
	signature = parseSignature(path);
}

/**
 *
 */
digidoc::Signature::~Signature()
{
}

/**
 * @return returns the created empty signature.
 */
std::auto_ptr<digidoc::dsig::SignatureType> digidoc::Signature::createEmptySignature()
{
    // Signature->SignedInfo
    dsig::CanonicalizationMethodType canonicalizationMethod(xml_schema::Uri("http://www.w3.org/TR/2001/REC-xml-c14n-20010315"));
    dsig::SignatureMethodType signatureMethod(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#rsasha1"));
    dsig::SignedInfoType signedInfo(canonicalizationMethod, signatureMethod);

    // Signature->SignatureValue
    digidoc::dsig::SignatureValueType signatureValue;

    // root (Signature)
    return std::auto_ptr<dsig::SignatureType>(new dsig::SignatureType(signedInfo, signatureValue));
}

std::auto_ptr<digidoc::xades::QualifyingPropertiesType> digidoc::Signature::createXAdES()
{
    DEBUG("digidoc::Signature::createXAdES");

//QualifingProperties->SignedProperties->SignedSignatureProperties
    digidoc::xades::SignedSignaturePropertiesType sSignatureProperties;
    sSignatureProperties.id(xml_schema::Id("S0-SIG"));

//QualifingProperties->SignedProperties->SignatureProductionPlace
    digidoc::xades::SignatureProductionPlaceType place;
    place.countryName("Estonia");
    place.city("Tallinn");
    place.postalCode("10122");
    place.stateOrProvince("Harjumaa");
    sSignatureProperties.signatureProductionPlace(place);

//QualifingProperties->SignedProperties->SignerRole
    digidoc::xades::SignerRoleType signerRole;
    digidoc::xades::ClaimedRolesListType claimedRolesList;

    /*
    //FIXME: teeb kooriku
    XMLCh tempStr[100];
    xercesc::DOMNode * node = claimedRolesList._node();
    xercesc::DOMDocument * domDoc =  node->getOwnerDocument();

    xercesc::XMLString::transcode("ClaimedRole", tempStr, 99);
    xercesc::DOMElement * role = domDoc->createElement(tempStr);

    xercesc::XMLString::transcode("Agreed", tempStr, 99);
    role->setTextContent(tempStr);

    //create "ClaimedRole" anyType using DOMElement
    digidoc::xades::AnyType claimed(*role);
    */

    //digidoc::xades::AnyType claimed;
    //claimedRolesList.claimedRole().push_back(claimed);

    signerRole.claimedRoles(claimedRolesList);
    sSignatureProperties.signerRole(signerRole);

///QualifingProperties->SignedProperties->SignedSignatureProperties->SigningCertificate

    unsigned char sign[20];
    xml_schema::Base64Binary b64Digest(sign, 20);
    std::auto_ptr<xades::CertIDListType> certList(createXAdESCertIDListType("http://www.w3.org/2000/09/xmldsig#sha1",
        b64Digest,
        std::string("emailAddress=pki@sk.ee,C=EE,O=AS Sertifitseerimiskeskus,OU=ESTEID,SN=1,CN=ESTEIDSK"),
        1163490760));

    sSignatureProperties.signingCertificate(*certList);

//QualifingProperties->SignedProperties->SigningTime
    std::time_t* t = new time_t();
    time(t);
    struct tm *lt = localtime(t);

    //FIXME: broken, time is empty
    xml_schema::DateTime now(lt->tm_year + 1900,
            lt->tm_mday + 1,
            lt->tm_mday,
            lt->tm_hour,
            lt->tm_min,
            lt->tm_sec);

    sSignatureProperties.signingTime(now);

//QualifingProperties->SignedProperties
    digidoc::xades::SignedPropertiesType signedProperties(sSignatureProperties);
    signedProperties.id( xml_schema::Id("S0-SignedProperties") );

//XAdES root (QualifyingProperties)
    std::auto_ptr<digidoc::xades::QualifyingPropertiesType> qualifyingProperties =
        std::auto_ptr<digidoc::xades::QualifyingPropertiesType>(
             new digidoc::xades::QualifyingPropertiesType (
                 xml_schema::Uri("http://uri.etsi.org/01903/v1.3.2#")));
    qualifyingProperties->target(xml_schema::Uri("#S0") );
    qualifyingProperties->signedProperties(signedProperties);

/*
    std::cout << "QualifyingProperties:" << qualifyingProperties << std::endl;
    std::string fileName = util::File::tempFileName();
    DEBUG("Serializing XAdES XML to '%s'", fileName.c_str())
    // Serialize XML to file.
    xml_schema::NamespaceInfomap map;
    map["xades"].name = "http://uri.etsi.org/01903/v1.3.2#";
    std::ofstream ofs(fileName.c_str());
    digidoc::xades::QualifyingProperties(ofs, qualifyingProperties, map);
    ofs.close();
    std::cout << "Created file " << fileName << std::endl;
*/
    return qualifyingProperties;
}

digidoc::xades::CertIDListType * digidoc::Signature::createXAdESCertIDListType(
    const std::string& digestMethodAlgorithmUri,
    const xml_schema::Base64Binary& digestValueB64,
    const std::string& X509IssuerName,
    long long issuerSerialNumber)
{
    digidoc::xades::CertIDListType* certList = new digidoc::xades::CertIDListType();
    xml_schema::Uri uri(digestMethodAlgorithmUri);
    digidoc::dsig::DigestMethodType digestMethod( uri );

    digidoc::dsig::DigestValueType digestValue(digestValueB64);

    digidoc::xades::DigestAlgAndValueType digestAlgAndValue(digestMethod, digestValue);

    xml_schema::String issuerName(X509IssuerName);
    digidoc::dsig::X509IssuerSerialType x509IssuerSerial(issuerName, issuerSerialNumber);
    digidoc::xades::CertIDType cert(digestAlgAndValue, x509IssuerSerial);


    certList->cert().push_back(cert);

    return certList;
}

std::auto_ptr<digidoc::xades::UnsignedPropertiesType> digidoc::Signature::createXAdESUnsignedProperties()
{
    std::auto_ptr<digidoc::xades::UnsignedPropertiesType> root(new digidoc::xades::UnsignedPropertiesType() );
    ::digidoc::xades::UnsignedSignaturePropertiesType sigProperties;

//UnsignedProperties->UnsignedSignatureProperties->CompleteCertificateRefs
    unsigned char sign[20];
    xml_schema::Base64Binary b64Digest(sign, 20);
    std::auto_ptr<xades::CertIDListType> certList(
            createXAdESCertIDListType(std::string("http://www.w3.org/2000/09/xmldsig#sha1"),
            b64Digest,
            std::string("emailAddress=pki@sk.ee,C=EE,O=AS Sertifitseerimiskeskus,OU=ESTEID,SN=1,CN=ESTEIDSK"),
            1163490760));

    xades::CompleteCertificateRefsType certRefs(certList);
    sigProperties.completeCertificateRefs().push_back(certRefs);

//UnsignedProperties->UnsignedSignatureProperties->CompleteRevocationRefs

    xades::ResponderIDType responderIDType;
    responderIDType.byName(xml_schema::String("C=EE,O=ESTEID,OU=OCSP,CN=ESTEID-SK 2007 OCSP RESPONDER,emailAddress=pki@sk.ee"));

    unsigned char hash[20];
    xml_schema::Base64Binary b64Hash(hash, 20);

    digidoc::dsig::DigestValueType digestValue(b64Hash);
    dsig::DigestMethodType digestMethod(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1"));

    digidoc::xades::DigestAlgAndValueType digestAlgAndValue(digestMethod, digestValue);

    //FIXME: date
    xades::OCSPIdentifierType ocspId(responderIDType,  xml_schema::DateTime(0,0,0,0,0,0) );
    ocspId.uRI(xml_schema::Uri("#N0") );

    digidoc::xades::CompleteRevocationRefsType revocationRefs;
    digidoc::xades::OCSPRefsType ocspRefs;
    digidoc::xades::OCSPRefType ocspRef(ocspId);

    ocspRef.digestAlgAndValue(digestAlgAndValue);

    ocspRefs.oCSPRef().push_back(ocspRef);
    revocationRefs.oCSPRefs(ocspRefs);
    sigProperties.completeRevocationRefs().push_back(revocationRefs);

//UnsignedProperties->UnsignedSignatureProperties->SigAndRefsTimeStamp
    digidoc::xades::XAdESTimeStampType timeStamp;
    timeStamp.id( xml_schema::Id("S0-T1") );

    digidoc::xades::IncludeType inc1(xml_schema::Uri("#S0-SIG"));
    timeStamp.include().push_back(inc1);

    unsigned char timeStampBytes[32];
    xml_schema::Base64Binary b64TimeStamp(timeStampBytes, 32);
    digidoc::xades::EncapsulatedPKIDataType encTimeStamp(b64TimeStamp);
    timeStamp.encapsulatedTimeStamp().push_back(encTimeStamp);

    sigProperties.sigAndRefsTimeStamp().push_back(timeStamp);

//UnsignedProperties->UnsignedSignatureProperties->CertificateValues
    digidoc::xades::CertificateValuesType certValues;
    unsigned char certBytes1[30];
    digidoc::xades::EncapsulatedPKIDataType encX509Cert1(xml_schema::Base64Binary(certBytes1, 30));
    encX509Cert1.id(xml_schema::Id("S0-CA-CERT"));
    certValues.encapsulatedX509Certificate().push_back(encX509Cert1);
    sigProperties.certificateValues().push_back(certValues);

//UnsignedProperties->UnsignedSignatureProperties->RevocationValues
    digidoc::xades::RevocationValuesType revocationValues;
    digidoc::xades::OCSPValuesType ocspValues;
    unsigned char ocspBytes1[30];
    digidoc::xades::EncapsulatedPKIDataType ocspValue1(xml_schema::Base64Binary(ocspBytes1, 30));
    ocspValue1.id( xml_schema::Id("N0") );

    ocspValues.encapsulatedOCSPValue().push_back(ocspValue1);
    revocationValues.oCSPValues(ocspValues);

    sigProperties.revocationValues().push_back(revocationValues);

    return root;
}

/**
 *
 * @param path
 * @return
 */
std::auto_ptr<digidoc::dsig::SignatureType> digidoc::Signature::parseSignature(const std::string& path)
{
    std::string fileName(path);

    // verify if well-formed against xades xsd
    xml_schema::Properties properties;
    properties.schema_location(XADES_NAMESPACE, Conf::getInstance()->getXadesXsdPath());
    properties.schema_location(DSIG_NAMESPACE, Conf::getInstance()->getDsigXsdPath());
    std::auto_ptr<dsig::SignatureType> sig(dsig::signature(fileName, 0, properties));

    signature = sig;

    return signature;
}

/**
 * Adds artifact digest value as reference in the signature.
 *
 * @param uri reference URI.
 * @param digestMethod digest method (e.g NID_sha1 for SHA1, see openssl/obj_mac.h). Currently only
 *        SHA1 is supported.
 * @param digestValue digest value.
 * @throws SignatureException throws exception if the digest method is not supported.
 */
void digidoc::Signature::addReference(const std::string& uri, int digestMethod,
		std::vector<unsigned char> digestValue) throw(SignatureException)
{
	if(digestMethod != NID_sha1)
	{
		THROW_SIGNATUREEXCEPTION("Unsupported digest method '%s', only SHA1 is supported.", OBJ_nid2sn(digestMethod))
	}

    dsig::DigestMethodType method(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1"));
    dsig::DigestValueType value(xml_schema::Base64Binary(&digestValue[0], digestValue.size()));
    digidoc::dsig::ReferenceType reference(method, value);
    reference.uRI(xml_schema::Uri(uri));

    signature->signedInfo().reference().push_back(reference);
}

void digidoc::Signature::setSigningCertificate(X509* cert)
{
    DEBUG("digidoc::Signature::setSigningCertificate()");


#if 1
    BIO *out = NULL;
    // open BIO for output to 'stdout'
    if (!(out = BIO_new_fp(stdout, BIO_NOCLOSE)))
    {
        ERROR("Error creating OpenSSL BIO");
    }

    X509_print(out, cert);
    BIO_free_all(out);
#endif


    //cert.

    //Signature->KeyInfo->KeyValue


    int mod_len = 0;//modulus length in bytes
    int exp_len = 0;
    unsigned char rsaModulusBytes[512];
    unsigned char rsaExponentBytes[3];


//FIXME: extract ugly openSSL code to separate util
    EVP_PKEY * pubKey = X509_get_pubkey(cert);
    if (pubKey != NULL)
    {
        if(pubKey->type == EVP_PKEY_RSA)
        {

            mod_len = BN_num_bytes(pubKey->pkey.rsa->n);
            exp_len = BN_num_bytes(pubKey->pkey.rsa->e);

            DEBUG("Modulus len %d bytes", mod_len);
            DEBUG("Modulus len %d bytes", exp_len);

            //FIXME: check len

            BN_bn2bin(pubKey->pkey.rsa->n, &rsaModulusBytes[0]);//modulus
            BN_bn2bin(pubKey->pkey.rsa->e, &rsaExponentBytes[0]);//exponent
        }

        //EVP_PKEY_free(pubKey);
    }
    else
    {
        ERROR("Unable to load public Key");
    }

    digidoc::dsig::KeyValueType keyValue;

    digidoc::dsig::CryptoBinary modulus(xml_schema::Base64Binary(rsaModulusBytes, mod_len));
    digidoc::dsig::CryptoBinary exponent(xml_schema::Base64Binary(rsaExponentBytes, exp_len));

    digidoc::dsig::RSAKeyValueType rsaKey(modulus, exponent);
    keyValue.rSAKeyValue(rsaKey);


    DEBUG("Getting X509 serial");
    ASN1_INTEGER * asnSerialNum = X509_get_serialNumber(cert);
    long certSerial = ASN1_INTEGER_get(asnSerialNum);
    //ASN1_INTEGER_free(asnSerialNum);
    DEBUG("Getting X509 serial %ld", certSerial);

    /*
    DEBUG("Getting X509 issuer name");
    X509_NAME * x509IssuerName =  X509_get_issuer_name(cert);
    std::string issuerName( x509IssuerName->bytes->data );
    X509_NAME_free(x509IssuerName);
    DEBUG("X509 issuer name %s", x509IssuerName->bytes->data);
    */

    int certLength = 0;
    BIO *bio = NULL;
    //unsigned char *temp = NULL;
    std::vector<unsigned char> derEncodedX509;
    //unsigned char *derEncodedX509 = NULL;
    certLength = i2d_X509(cert, NULL);
    derEncodedX509.resize(certLength);

    unsigned char *pDummy = &derEncodedX509[0];
    certLength = i2d_X509(cert, &pDummy);

    DEBUG("Extracting DER X509");
    //get DER encoded X509 cert
    //internal 2 DER X509
    //temp is needed, see man i2d_X509
    //certLength = i2d_X509(cert, &derEncodedX509);
    DEBUG("X509 length %d", certLength);

    //derEncodedX509 = new unsigned char(certLength);

    //certLength = i2d_X509(cert, &derEncodedX509);

    if(certLength < 0){

        ERROR("Error decoding X509 cert");
    }

    ERROR("OpenSSL ERROR:%ld",ERR_peek_error());

    unsigned long errorCode;
    while((errorCode =  ERR_get_error()) != 0)
    {
        char errorMsg[250];
        ERR_error_string(errorCode, &errorMsg[0]);
        ERROR("OpenSSL ERROR:%ld %s",errorCode, errorMsg);
    }



    //std::string tempFile = util::File::tempFileName();
    //FILE* certFile = fopen(tempFile.c_str(), "w");
    //DEBUG("Writing cert to %s", tempFile.c_str());
    //i2d_X509_fp(certFile, cert);
    //fclose(certFile);
    //bio = BIO_new_mem_buf(derEncodedX509, certLength);
    //derEncodedX509 = temp;

    //DEBUG("Getting X509");



    DEBUG("Extracted DER X509 with length %d", certLength);

    xml_schema::Base64Binary pemX509(&derEncodedX509[0], certLength);

    //BIO_free_all(bio);



    //Signature->KeyInfo->X509Data
       //base64 encoding of a DER-encoded X.509 certificate = PEM encoded

        digidoc::dsig::X509DataType x509Data;
        x509Data.x509Certificate().push_back(pemX509);
        //std::cout << x509Data << std::endl;

    //Signature->KeyInfo
        digidoc::dsig::KeyInfoType keyInfoType;
        keyInfoType.keyValue().push_back(keyValue);
        keyInfoType.x509Data().push_back(x509Data);
        //std::cout << "KeyInfo::" << std::endl << keyInfoType << std::endl;

/*
    //Signature->SignatureValue
        unsigned char sign[20];
        xml_schema::Base64Binary b64Sign(sign, 20);
        digidoc::dsig::SignatureValueType signatureValue(b64Sign);
        signatureValue.Id(xml_schema::id("S0-SIG"));

    //Signature->SignedInfo
        digidoc::dsig::CanonicalizationMethodType canonicalizationMethod( xml_schema::uri("http://www.w3.org/TR/2001/REC-xml-c14n-20010315") );
        digidoc::dsig::SignatureMethodType signatureMethod( xml_schema::uri("http://www.w3.org/2000/09/xmldsig#rsasha1") );
        digidoc::dsig::SignedInfoType signedInfo(canonicalizationMethod, signatureMethod);
        digidoc::dsig::DigestMethodType dmeth1(xml_schema::uri("http://www.w3.org/2000/09/xmldsig#sha1"));
        unsigned char sha1__001[20];
        digidoc::dsig::DigestValueType dval1(xml_schema::Base64Binary(sha1__001, 20));
        digidoc::dsig::ReferenceType ref1(dmeth1, dval1);
        ref1.URI(xml_schema::uri("/document.doc"));
        signedInfo.Reference().push_back(ref1);

    //root (Signature)
        digidoc::dsig::SignatureType signature(signedInfo, signatureValue);
        signature.Id("S0");
*/

        signature->keyInfo(keyInfoType);

}

/**
 *
 * @param signatureProductionPlace
 */
void digidoc::Signature::setSignatureProductionPlace(const Signer::SignatureProductionPlace& signatureProductionPlace)
{
	DEBUG("setSignatureProductionPlace(signatureProductionPlace={city='%s',stateOrProvince='%s',postalCode='%s',countryName='%s'})", signatureProductionPlace.city.c_str(), signatureProductionPlace.countryName.c_str(), signatureProductionPlace.postalCode.c_str(), signatureProductionPlace.stateOrProvince.c_str())

    // TODO: implement

	//signature->Object()[0];

	xades::SignatureProductionPlaceType spp;
	spp.city(signatureProductionPlace.city);
	spp.stateOrProvince(signatureProductionPlace.stateOrProvince);
	spp.postalCode(signatureProductionPlace.postalCode);
	spp.countryName(signatureProductionPlace.countryName);

	xades::SignedSignaturePropertiesType ssp;
	ssp.signatureProductionPlace(spp);
    xades::SignedPropertiesType sp(ssp);
    xades::QualifyingPropertiesType qpt(xml_schema::Uri("http://uri.etsi.org/01903/v1.3.2#"));
    qpt.signedProperties(sp);

    dsig::ObjectType object;
    //object.Id("OBJ-ID");             // optional
    //object.Encoding("OBJ-ENCODING"); // optional
    //object.MimeType("OBJ-MIMETYPE"); // optional
    object.qualifyingProperties().push_back(qpt);

    //dsig::X509Data d;

    DEBUG("signature->Object().size() = %d", signature->object().size())
}

/**
 *
 * @param signerRole
 */
void digidoc::Signature::setSignerRole(const Signer::SignerRole& signerRole)
{
	DEBUG("setSignerRole(signerRole={claimedRoles={'%s'}})", signerRole.claimedRoles[0].c_str())
    // TODO: implement
}

/**
 * Saves signature to file using XAdES XML format.
 *
 * @return returns path of the signature XML.
 * @throws IOException throws exception if the signature file creation failed.
 */
std::string digidoc::Signature::saveToXml() throw(IOException)
{
	if(!path.empty())
	{
        return path;
	}

    // Serialize XML to file.
	path = util::File::tempFileName();
	DEBUG("Serializing XML to '%s'", path.c_str())

    xml_schema::NamespaceInfomap map;
    map["ds"].name = "http://www.w3.org/2000/09/xmldsig#";

	// TODO: remove printing to std out.
	//std::cout << "Signature:" << signature << std::endl;
    INFO("------------------------------------------------------------------------------------------------------------")
    digidoc::dsig::signature(std::cout, *signature, map);
    INFO("------------------------------------------------------------------------------------------------------------")

    std::ofstream ofs(path.c_str());
    dsig::signature(ofs, *signature, map);
    ofs.close();

    if(ofs.fail())
    {
        THROW_IOEXCEPTION("Failed to create signature XML file to '%s'.", path.c_str());
    }

    return path;
}


/**
 * The address where was the signature given.
 *
 * @return returns structure containing the address of signing place.
 */
digidoc::Signer::SignatureProductionPlace digidoc::Signature::getProductionPlace() const
{
    Signer::SignatureProductionPlace productionPlace;

    const xades::SignedSignaturePropertiesType& signedSigProps = getSignedSignatureProperties();

    const xades::SignedSignaturePropertiesType::SignatureProductionPlaceOptional& sigProdPlaceOptional =
        signedSigProps.signatureProductionPlace();
    if ( !sigProdPlaceOptional.present() )
    {
        return productionPlace; // it's optional block, so return empty. TODO: throw instead?
    }

    const xades::SignatureProductionPlaceType& sigProdPlace = sigProdPlaceOptional.get();

    // the fields are optional too
    if ( sigProdPlace.city().present() )
        productionPlace.city = sigProdPlace.city().get();
    if ( sigProdPlace.stateOrProvince().present() )
        productionPlace.stateOrProvince = sigProdPlace.stateOrProvince().get();
    if ( sigProdPlace.postalCode().present() )
        productionPlace.postalCode = sigProdPlace.postalCode().get();
    if ( sigProdPlace.countryName().present() )
        productionPlace.countryName = sigProdPlace.countryName().get();

    return productionPlace;
}

/**
 * The role that signer claims to hold while signing.
 *
 * @return returns the claimed role of the signer.
 */
digidoc::Signer::SignerRole digidoc::Signature::getSignerRole() const
{
    Signer::SignerRole roles;

    // SignedSignatureProperties
    const xades::SignedSignaturePropertiesType& signedSigProps =
        getSignedSignatureProperties();

    // SignerRole
    const xades::SignedSignaturePropertiesType::SignerRoleOptional& roleOpt =
        signedSigProps.signerRole();
    if ( !roleOpt.present() )
    {
        return roles; // it's optional block, so return empty. TODO: throw instead?
    }
    const xades::SignerRoleType& signerRole = roleOpt.get();

    // ClaimedRole
    const xades::SignerRoleType::ClaimedRolesOptional& claimedRoleOpt = signerRole.claimedRoles();
    if ( !claimedRoleOpt.present() )
    {
        return roles; // it's optional block, so return empty. TODO: throw instead? Xades vs. BDoc
    }
    // ClaimedRole - list
    const xades::ClaimedRolesListType& claimedRolesList = claimedRoleOpt.get();
    // ClaimedRole - sequence
    const xades::ClaimedRolesListType::ClaimedRoleSequence& claimedRolesSequence =
        claimedRolesList.claimedRole();

    for ( xades::ClaimedRolesListType::ClaimedRoleSequence::const_iterator itRoles =
            claimedRolesSequence.begin()
        ; itRoles != claimedRolesSequence.end()
        ; itRoles++ )
    {
        roles.claimedRoles.push_back(*itRoles);
    }

    return roles;
}

/**
* The role that signer claims to hold while signing.
*
* @return returns the claimed role of the signer.
*/
std::string digidoc::Signature::getSigningTime() const
{
    std::string signingTime;

    const xades::SignedSignaturePropertiesType& signedSigProps = getSignedSignatureProperties();

    const xades::SignedSignaturePropertiesType::SigningTimeOptional& sigTimeOpt =
        signedSigProps.signingTime();
    if ( !sigTimeOpt.present() )
    {
        return ""; // it's optional, so return empty. TODO: throw instead?
    }
    const xades::SignedSignaturePropertiesType::SigningTimeType& sigTime = sigTimeOpt.get();

    signingTime = util::date::xsd2string(sigTime);

    return signingTime;
}

/**
 * Signer certificate taken from current signature. 
 *
 * @return returns the SignedSignaturePropertiesType object.
*/
X509* digidoc::Signature::getSigningCertificate() const
{
    X509* cert = NULL;

    // KeyInfo
    const dsig::SignatureType::KeyInfoOptional& keyInfoOptional = signature->keyInfo();
    if ( !keyInfoOptional.present() )
    {
        THROW_SIGNATUREEXCEPTION("Signature does not contain signer certificate");
    }
    const dsig::KeyInfoType& keyInfo = keyInfoOptional.get();

    // X509Data
    const dsig::KeyInfoType::X509DataSequence& x509DataSeq = keyInfo.x509Data();

    // X509Certificate
    if ( x509DataSeq.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature does not contain signer certificate");
    }
    else if ( x509DataSeq.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature contains more than one signers certificate");
    }
    const dsig::X509DataType& x509Data = x509DataSeq.front();
    const dsig::X509DataType::X509CertificateSequence& x509CertSeq = x509Data.x509Certificate();
    if ( x509CertSeq.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature does not contain signer certificate");
    }
    else if ( x509CertSeq.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature contains more than one signers certificate");
    }
    const dsig::X509DataType::X509CertificateType& certBase64Buf = x509CertSeq.front();

    //DEBUG(certBase64Buf.data());
    std::cout << "---------------------------------------" << std::endl;
    std::cout << certBase64Buf.encode() << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    //DEBUG(certBase64Buf.encode().c_str());
    
    // TODO

    return cert;
}

/**
* Helper that retrieves SignedSignatureProperties xades object. It will throw
* in case the block is not present.
*
* @return returns the SignedSignaturePropertiesType object.
*/
const digidoc::xades::SignedSignaturePropertiesType&
    digidoc::Signature::getSignedSignatureProperties() const  throw(SignatureException)
{
    // Object
    const dsig::SignatureType::ObjectSequence& os = signature->object();
    if ( os.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature block 'Object' is missing.");
    }
    else if ( os.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature block contains more than one 'Object' block.");
    }
    const dsig::ObjectType& o = os[0];

    // QualifyingProperties
    const dsig::ObjectType::QualifyingPropertiesSequence& qpSeq = o.qualifyingProperties();
    if ( qpSeq.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature block 'QualifyingProperties' is missing.");
    }
    else if ( qpSeq.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature block 'Object' contains more than one 'QualifyingProperties' block.");
    }
    const xades::QualifyingPropertiesType& qp = qpSeq[0];

    // SignedProperties
    const xades::QualifyingPropertiesType::SignedPropertiesOptional& signedPropsOptional =
        qp.signedProperties();

    if ( !signedPropsOptional.present() )
    {
        THROW_SIGNATUREEXCEPTION("QualifyingProperties block 'SignedProperties' is missing.");
    }
    const xades::SignedPropertiesType& signedProps = qp.signedProperties().get();

    // SignedSignatureProperties
    const xades::SignedSignaturePropertiesType& signedSigProps =
        signedProps.signedSignatureProperties();

    return signedSigProps;

}
