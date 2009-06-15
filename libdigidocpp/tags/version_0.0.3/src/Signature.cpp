#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include <openssl/objects.h>
#include <openssl/err.h>

#include <xsec/canon/XSECC14n20010315.hpp>

#include "log.h"
#include "Conf.h"
#include "Signature.h"
#include "util/File.h"
#include "util/String.h"
#include "util/DateTime.h"
#include "crypto/Digest.h"
#include "xml/OpenDocument_manifest.hxx"
#include "xml/xmldsig-core-schema.hxx"
#include "xml/XAdES.hxx"

const std::string digidoc::Signature::XADES_NAMESPACE = "http://uri.etsi.org/01903/v1.3.2#";
const std::string digidoc::Signature::DSIG_NAMESPACE = "http://www.w3.org/2000/09/xmldsig#";

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
    delete signature;
}

/**
 * @return returns the created empty signature.
 */
digidoc::dsig::SignatureType* digidoc::Signature::createEmptySignature()
{
    DEBUG("digidoc::Signature::createEmptySignature");
    // Signature->SignedInfo
    dsig::CanonicalizationMethodType canonicalizationMethod(xml_schema::Uri("http://www.w3.org/TR/2001/REC-xml-c14n-20010315"));
    dsig::SignatureMethodType signatureMethod(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#rsasha1"));
    dsig::SignedInfoType signedInfo(canonicalizationMethod, signatureMethod);

    // Signature->SignatureValue
    digidoc::dsig::SignatureValueType signatureValue;
    signatureValue.id(xml_schema::Id("S0-SIG"));

    // Signature->Object->QualifyingProperties->SignedProperties->SignedSignatureProperties
    xades::SignedSignaturePropertiesType signedSignatureProperties;

    // Signature->Object->QualifyingProperties->SignedProperties
    xades::SignedPropertiesType signedProperties(signedSignatureProperties);
    signedProperties.id(xml_schema::Id("S0-SignedProperties"));
    signedProperties.signedSignatureProperties(signedSignatureProperties);

    // Signature->Object->QualifyingProperties
    xades::QualifyingPropertiesType qualifyingProperties(xml_schema::Uri("#S0"));
    qualifyingProperties.signedProperties(signedProperties);

    // Signature->Object
    dsig::ObjectType object;
    object.qualifyingProperties().push_back(qualifyingProperties);

    // Signature (root)
    dsig::SignatureType* signature = new dsig::SignatureType(signedInfo, signatureValue);
    signature->id(xml_schema::Id("S0"));
    signature->object().push_back(object);

    return signature;
}

/**
 *
 * @param path
 * @return
 */
digidoc::dsig::SignatureType* digidoc::Signature::parseSignature(const std::string& path)
{
    std::string fileName(path);

    // verify if well-formed against xades xsd
    xml_schema::Properties properties;
    properties.schema_location(XADES_NAMESPACE, Conf::getInstance()->getXadesXsdPath());
    properties.schema_location(DSIG_NAMESPACE, Conf::getInstance()->getDsigXsdPath());
    dsig::SignatureType* sig = dsig::signature(fileName, 0, properties).release();



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
		std::vector<unsigned char> digestValue, const std::string& type) throw(SignatureException)
{
	if(digestMethod != NID_sha1)
	{
		THROW_SIGNATUREEXCEPTION("Unsupported digest method '%s', only SHA1 is supported.", OBJ_nid2sn(digestMethod));
	}

    dsig::DigestMethodType method(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1"));
    dsig::DigestValueType value(xml_schema::Base64Binary(&digestValue[0], digestValue.size()));
    digidoc::dsig::ReferenceType reference(method, value);
    reference.uRI(xml_schema::Uri(uri));

    if(!type.empty())
    {
        reference.type(type);
    }

    signature->signedInfo().reference().push_back(reference);
}

/**
 * Adds signing certificate to the signature XML. The DER encoded X.509 certificate is added to
 * Signature->KeyInfo->X509Data->X509Certificate. Certificate info is also added to
 * Signature->Object->QualifyingProperties->SignedProperties->SignedSignatureProperties->SigningCertificate.
 *
 * @param cert certificate that is used for signing the signature XML.
 */
void digidoc::Signature::setSigningCertificate(const X509Cert& x509)
{
    DEBUG("digidoc::Signature::setSigningCertificate()");

    // Add the provided certificate to XML tree.
    // Signature->KeyInfo->X509Data->X509Certificate
    std::vector<unsigned char> derEncodedX509 = x509.encodeDER();

    // BASE64 encoding of a DER-encoded X.509 certificate = PEM encoded.
    xml_schema::Base64Binary pemX509(&derEncodedX509[0], derEncodedX509.size());

    dsig::X509DataType x509Data;
    x509Data.x509Certificate().push_back(pemX509);

    dsig::KeyInfoType keyInfo;
    keyInfo.x509Data().push_back(x509Data);

    signature->keyInfo(keyInfo);


    // Add the provided certificate info to XML tree.
    // Signature->Object->QualifyingProperties->SignedProperties->SignedSignatureProperties->SigningCertificate

    // TODO: How to calculate digest for X.509 cert?
    // Calculate SHA1 digest of the certificate.
    SHA1Digest certSha1;
    certSha1.update(&derEncodedX509[0], derEncodedX509.size());
    dsig::DigestMethodType digestMethod(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1"));
    dsig::DigestValueType digestValue(xml_schema::Base64Binary(&certSha1.getDigest()[0], SHA1Digest::DIGEST_SIZE));
    xades::DigestAlgAndValueType certDigest(digestMethod, digestValue);

    // Add certificate issuer info.
    dsig::X509IssuerSerialType issuerSerial(xml_schema::String(x509.getCertIssuerName()), xml_schema::Integer(x509.getCertSerial()));
    xades::SignedSignaturePropertiesType::SigningCertificateType::CertType _cert(certDigest, issuerSerial);

    xades::CertIDListType signingCertificate;
    signingCertificate.cert().push_back(_cert);

    signature->object()[0].qualifyingProperties()[0].signedProperties()
            ->signedSignatureProperties().signingCertificate(signingCertificate);
}

/**
 * Sets signature production place.
 *
 * @param spp signature production place.
 */
void digidoc::Signature::setSignatureProductionPlace(const Signer::SignatureProductionPlace& spp)
{
	DEBUG("setSignatureProductionPlace(spp={city='%s',stateOrProvince='%s',postalCode='%s',countryName='%s'})", spp.city.c_str(), spp.countryName.c_str(), spp.postalCode.c_str(), spp.stateOrProvince.c_str())

    xades::SignatureProductionPlaceType signatureProductionPlace;
	signatureProductionPlace.city(spp.city);
	signatureProductionPlace.stateOrProvince(spp.stateOrProvince);
	signatureProductionPlace.postalCode(spp.postalCode);
	signatureProductionPlace.countryName(spp.countryName);

    signature->object()[0].qualifyingProperties()[0].signedProperties()
            ->signedSignatureProperties().signatureProductionPlace(signatureProductionPlace);
}

/**
 * Sets signer claimed roles to the signature.
 * NB! Only ClaimedRoles are supported. CerifiedRoles are not supported.
 *
 * @param roles signer roles.
 */
void digidoc::Signature::setSignerRole(const Signer::SignerRole& roles)
{
	//DEBUG("setSignerRole(signerRole={claimedRoles={'%s'}})", roles.claimedRoles[0].c_str())

	xades::ClaimedRolesListType claimedRoles;
	for(std::vector<std::string>::const_iterator iter = roles.claimedRoles.begin(); iter != roles.claimedRoles.end(); iter++)
	{
	    claimedRoles.claimedRole().push_back(xml_schema::String(*iter));
	}

	xades::SignerRoleType signerRole;
	signerRole.claimedRoles(claimedRoles);
    signature->object()[0].qualifyingProperties()[0].signedProperties()
            ->signedSignatureProperties().signerRole(signerRole);
}

/**
 * Sets signature signing time.
 *
 * @param signingTime signing time.
 */
void digidoc::Signature::setSigningTime(const xml_schema::DateTime& signingTime)
{
    signature->object()[0].qualifyingProperties()[0].signedProperties()
            ->signedSignatureProperties().signingTime(signingTime);
}

/**
 *
 * @param sigValue
 */
void digidoc::Signature::setSignatureValue(Signer::Signature sigValue)
{
	dsig::SignatureValueType signatureValue(xml_schema::Base64Binary(sigValue.signature, sigValue.length));
	dsig::SignatureValueType::IdOptional id;
	//xml_schema::Id i = id.get();

	//signatureValue.id(signature->signatureValue().id());
	//signature->signatureValue().id().reset();
	signature->signatureValue(signatureValue);


	//_signatureValue.buffer(signatureValue.signature, signatureValue.length);

	//signature->signatureValue(_signatureValue);
	//signature->signatureValue().buffer(signatureValue.signature, signatureValue.length);

	signature->signatureValue();

    // TODO: implement
	DEBUG("TODO: implement setSignatureValue()")
}

/**
 * @return returns Xerces DOM document created from current signature
 */
xml_schema::dom::auto_ptr<xercesc::DOMDocument> digidoc::Signature::createDom() const
{
    return dsig::signature(*signature, createNamespaceMap());
}

/**
 * Canonicalizes the XML node and calculates digest of it.
 *
 * @param node node to be digested.
 * @return returns digest of the XMl node.
 */
std::vector<unsigned char> digidoc::Signature::calcSHA1OnNode(xercesc::DOMNode* node)
{
	std::vector<unsigned char> c14n = c14nNode(node);
	DEBUG("c14n = '%s'", reinterpret_cast<char*>(&c14n[0]) )

    digidoc::SHA1Digest sha1;
    sha1.update(&c14n[0], c14n.size());
    return sha1.getDigest();
}

/**
 * Canonicalizes the XML node using Apache XML Security implementation.
 * Uses canonicalization method 'http://www.w3.org/TR/2001/REC-xml-c14n-20010315'.
 *
 * @param node node that is being canonicalized.
 * @return returns the Xerces node canonicalized to buffer.
 */
std::vector<unsigned char> digidoc::Signature::c14nNode(xercesc::DOMNode* node)
{
    XSECC14n20010315 canonicalizer(node->getOwnerDocument());
   	//canonicalizer.setCommentsProcessing(true);
   	//canonicalizer.setUseNamespaceStack(true);
    canonicalizer.setStartNode(node);

	std::vector<unsigned char> c14n;
	unsigned char buffer[1024];
    int bytes = 0;
	while((bytes = canonicalizer.outputBuffer(buffer, 1024)) > 0)
	{
		// XXX: use memcopy instead?
         for(int i = 0; i < bytes; i++)
         {
             c14n.push_back(buffer[i]);
         }
	}

	return c14n;
}

/**
 *
 * @return
 */
std::vector<unsigned char> digidoc::Signature::createNonce() const
{
    dsig::SignatureValueType& sigValue = signature->signatureValue();
    if(sigValue.size() == 0)
    {
        // XXX: Throw exception?
    }

    // XXX: Select digest method?
    SHA1Digest digest;
    digest.update(reinterpret_cast<const unsigned char*>(sigValue.data()), sigValue.size());
    return digest.getDigest();
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

    std::ofstream ofs(path.c_str());
    dsig::signature(ofs, *signature, createNamespaceMap());
    ofs.close();

    if(ofs.fail())
    {
        THROW_IOEXCEPTION("Failed to create signature XML file to '%s'.", path.c_str());
    }

	// TODO: remove printing to std out.
	//std::cout << "Signature:" << signature << std::endl;
    INFO("------------------------------------------------------------------------------------------------------------")
    dsig::signature(std::cout, *signature, createNamespaceMap());
    INFO("------------------------------------------------------------------------------------------------------------")

    return path;
}

/**
 * @return returns namespace map with dsig and xades namespaces.
 */
xml_schema::NamespaceInfomap digidoc::Signature::createNamespaceMap()
{
    xml_schema::NamespaceInfomap map;
    map["ds"].name = DSIG_NAMESPACE;
    map[""].name = XADES_NAMESPACE;
    return map;
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

// FIXME: return date object not string.
// FIXME: wrong comments
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

// FIXME: This method is not working.
/**
 * Signer certificate taken from current signature.
 *
 * @return returns the SignedSignaturePropertiesType object.
*/
digidoc::X509Cert digidoc::Signature::getSigningCertificate() const
{
    const dsig::X509DataType::X509CertificateType& certBlock = getSigningX509CertificateType();

    // TODO: use directly X509Cert constructor and move error handling to there
    X509* x509Ptr = NULL;

    // d2i_X509 will 1) require binary form and 2) offet the input pointer away
    const unsigned char* pDummy = reinterpret_cast< const unsigned char* >(certBlock.data());
    x509Ptr = d2i_X509(&x509Ptr, &pDummy, certBlock.size());

    if ( x509Ptr == NULL )
    {
        THROW_SIGNATUREEXCEPTION("Signer's certificate is invalid");
    }
    unsigned long errorCode = ERR_peek_error();
    if ( errorCode != 0 )
    {
        while( (errorCode = ERR_get_error()) != 0 )
        {
            char errorMsg[250];
            ERR_error_string(errorCode, &errorMsg[0]);
            ERR("OpenSSL ERROR:%ld %s",errorCode, errorMsg);
        }
        THROW_SIGNATUREEXCEPTION("Signer's certificate is invalid");
    }

    return X509Cert(x509Ptr);
}

/**
 * Get xades object for the certificate of the signer.
 *
 * @return returns the one and only X509CertificateType object.
 */
digidoc::dsig::X509DataType::X509CertificateType& digidoc::Signature::getSigningX509CertificateType()
const throw(SignatureException)
{
    // KeyInfo
    dsig::SignatureType::KeyInfoOptional& keyInfoOptional = signature->keyInfo();
    if ( !keyInfoOptional.present() )
    {
        THROW_SIGNATUREEXCEPTION("Signature does not contain signer certificate");
    }
    dsig::KeyInfoType& keyInfo = keyInfoOptional.get();

    // X509Data
    dsig::KeyInfoType::X509DataSequence& x509DataSeq = keyInfo.x509Data();
    if ( x509DataSeq.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature does not contain signer certificate");
    }
    else if ( x509DataSeq.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature contains more than one signers certificate");
    }
    dsig::X509DataType& x509Data = x509DataSeq.front();

    // X509Certificate
    dsig::X509DataType::X509CertificateSequence& x509CertSeq = x509Data.x509Certificate();
    if ( x509CertSeq.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature does not contain signer certificate");
    }
    else if ( x509CertSeq.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature contains more than one signers certificate");
    }
    dsig::X509DataType::X509CertificateType& certBase64Buf = x509CertSeq.front();

    return certBase64Buf;
}

/**
* Helper that retrieves SignedSignatureProperties xades object. It will throw
* in case the block is not present.
*
* @return returns the SignedSignaturePropertiesType object.
*/
digidoc::xades::SignedSignaturePropertiesType&
    digidoc::Signature::getSignedSignatureProperties() const  throw(SignatureException)
{
    // Object
    dsig::SignatureType::ObjectSequence& os = signature->object();
    if ( os.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature block 'Object' is missing.");
    }
    else if ( os.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature block contains more than one 'Object' block.");
    }
    dsig::ObjectType& o = os[0];

    // QualifyingProperties
    dsig::ObjectType::QualifyingPropertiesSequence& qpSeq = o.qualifyingProperties();
    if ( qpSeq.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature block 'QualifyingProperties' is missing.");
    }
    else if ( qpSeq.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Signature block 'Object' contains more than one 'QualifyingProperties' block.");
    }
    xades::QualifyingPropertiesType& qp = qpSeq[0];

    // SignedProperties
    xades::QualifyingPropertiesType::SignedPropertiesOptional& signedPropsOptional =
        qp.signedProperties();

    if ( !signedPropsOptional.present() )
    {
        THROW_SIGNATUREEXCEPTION("QualifyingProperties block 'SignedProperties' is missing.");
    }
    xades::SignedPropertiesType& signedProps = qp.signedProperties().get();

    // SignedSignatureProperties
    xades::SignedSignaturePropertiesType& signedSigProps =
        signedProps.signedSignatureProperties();

    return signedSigProps;

}
