#include "log.h"
#include "Conf.h"
#include "Signature.h"
#include "crypto/cert/X509Cert.h"
#include "crypto/Digest.h"
#include "util/File.h"
#include "util/String.h"
#include "util/DateTime.h"
#include "xml/OpenDocument_manifest.hxx"
#include "xml/xmldsig-core-schema.hxx"
#include "xml/XAdES.hxx"

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include <openssl/err.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <xsec/canon/XSECC14n20010315.hpp>

const std::string digidoc::Signature::XADES_NAMESPACE = "http://uri.etsi.org/01903/v1.3.2#";
const std::string digidoc::Signature::DSIG_NAMESPACE = "http://www.w3.org/2000/09/xmldsig#";

/**
 * Creates an new empty signature.
 */
digidoc::Signature::Signature()
 : signature(NULL)
 , path("")
 , isNew(true)
{
    signature = createEmptySignature();
}

/**
 * Parses signature from XML file.
 *
 * @param path path to signature XML file.
 * @throws SignatureException exception is thrown if the signature parsing failed
 *         (e.g. not a xml file, invalid xml file, etc).
 */
digidoc::Signature::Signature(const std::string& path) throw(SignatureException)
 : signature(NULL)
 , path(path)
 , isNew(false)
{
	try
	{
        signature = parseSignature(path);
	}
	catch(const IOException& e)
	{
		THROW_SIGNATUREEXCEPTION_CAUSE(e, "Failed to parse signature XML.");
	}
}

/**
 * Release signature.
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
    dsig::SignatureMethodType signatureMethod(xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#rsa-sha1"));//XXX: static const or dynamic
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
 * Parses signature from XML file.
 *
 * @param path path to signature XML file.
 * @return returns the parsed signature object.
 * @throws IOException exception is thrown if the signature parsing failed
 *         (e.g. not a xml file, invalid xml file, etc).
 */
digidoc::dsig::SignatureType* digidoc::Signature::parseSignature(const std::string& path) throw(IOException)
{
    try
    {
        // Validate against XML DSig and XAdES schemas.
        xml_schema::Properties properties;
        properties.schema_location(XADES_NAMESPACE, Conf::getInstance()->getXadesXsdPath());
        properties.schema_location(DSIG_NAMESPACE, Conf::getInstance()->getDsigXsdPath());

        // Don't initialize Xerces.
        xml_schema::Flags flags = xml_schema::Flags::dont_initialize;

        // Parse signature.
        return dsig::signature(path, flags, properties).release();
    }
    catch(const xml_schema::Parsing& e)
    {
        std::ostringstream oss;
        oss << e;
        THROW_IOEXCEPTION("Failed to parse signature XML: %s", oss.str().c_str());
    }
    catch(const xsd::cxx::exception& e)
    {
        THROW_IOEXCEPTION("Failed to parse signature XML: %s", e.what());
    }
    return NULL;
}

/**
 * Adds artifact digest value as reference in the signature.
 *
 * @param uri reference URI.
 * @param digestUri digest method URI (e.g. 'http://www.w3.org/2000/09/xmldsig#sha1' for SHA1)
 * @param digestValue digest value.
 * @param type reference type, optional parameter, default no type is added to the reference.
 *        For example 'http://uri.etsi.org/01903/#SignedProperties' for signed properties
 *        reference.
 * @throws SignatureException throws exception if the digest method is not supported.
 */
void digidoc::Signature::addReference(const std::string& uri, const std::string& digestUri,
        std::vector<unsigned char> digestValue, const std::string& type) throw(SignatureException)
{
    dsig::DigestMethodType method(xml_schema::Uri(digestUri.c_str()));
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
    dsig::KeyInfoType keyInfo;

    // Signature->KeyInfo->KeyValue->RSAKeyValue
    std::vector<unsigned char> rsaModulus = x509.getRsaModulus();
    dsig::RSAKeyValueType::ModulusType modulus(xml_schema::Base64Binary(&rsaModulus[0], rsaModulus.size()));
    std::vector<unsigned char> rsaExponent = x509.getRsaExponent();
    dsig::RSAKeyValueType::ExponentType exponent(xml_schema::Base64Binary(&rsaExponent[0], rsaExponent.size()));
    dsig::RSAKeyValueType rsaKeyValue(modulus, exponent);
    dsig::KeyValueType keyValue;
    keyValue.rSAKeyValue(rsaKeyValue);
    keyInfo.keyValue().push_back((keyValue));

    // Signature->KeyInfo->X509Data->X509Certificate
    std::vector<unsigned char> derEncodedX509 = x509.encodeDER();

    // BASE64 encoding of a DER-encoded X.509 certificate = PEM encoded.
    xml_schema::Base64Binary pemX509(&derEncodedX509[0], derEncodedX509.size());
    dsig::X509DataType x509Data;
    x509Data.x509Certificate().push_back(pemX509);
    keyInfo.x509Data().push_back(x509Data);

    signature->keyInfo(keyInfo);


    // Signature->Object->QualifyingProperties->SignedProperties->SignedSignatureProperties->SigningCertificate
    // Calculate digest of the X.509 certificate.
    std::auto_ptr<Digest> digest = Digest::create();
    digest->update(derEncodedX509);
    dsig::DigestMethodType digestMethod(xml_schema::Uri(digest->getUri()));
    dsig::DigestValueType digestValue(xml_schema::Base64Binary(&digest->getDigest()[0], digest->getSize()));
    xades::DigestAlgAndValueType certDigest(digestMethod, digestValue);

    // Add certificate issuer info.
    dsig::X509IssuerSerialType issuerSerial(xml_schema::String(x509.getIssuerName()), xml_schema::Integer(x509.getSerial()));
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
    DEBUG("setSignatureProductionPlace(spp={city='%s',stateOrProvince='%s',postalCode='%s',countryName='%s'})", spp.city.c_str(), spp.countryName.c_str(), spp.postalCode.c_str(), spp.stateOrProvince.c_str());

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
    if(!claimedRoles.claimedRole().empty())
    {
        signerRole.claimedRoles(claimedRoles);
        signature->object()[0].qualifyingProperties()[0].signedProperties()
            ->signedSignatureProperties().signerRole(signerRole);
    }
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
 * Sets signature value.
 *
 * @param sigValue signature value.
 */
void digidoc::Signature::setSignatureValue(Signer::Signature sigValue)
{
    dsig::SignatureValueType signatureValue(xml_schema::Base64Binary(sigValue.signature, sigValue.length));

    // Make copy of current signature value id.
    std::string id(signature->signatureValue().id()->c_str());

    // Set new signature value.
    signature->signatureValue(signatureValue);

    // Set signature value id back to its old value.
    signature->signatureValue().id(std::string(id));
}

/**
 * @return returns signature value.
 */
std::vector<unsigned char> digidoc::Signature::getSignatureValue() const
{
    dsig::SignatureType::SignatureValueType signatureValueType = signature->signatureValue();

    std::vector<unsigned char> signatureValue(signatureValueType.size(), 0);
    memcpy(&signatureValue[0], signatureValueType.data(), signatureValueType.size());

    return signatureValue;
}

/**
 * Canonicalize XML node using Canonical XML 1.0 specification (http://www.w3.org/TR/2001/REC-xml-c14n-20010315).
 * Using Xerces for parsing XML to preserve the white spaces "as is" and get
 * the same digest value on XML node each time.
 *
 * @param calc digest calculator implementation.
 * @param ns signature tag namespace.
 * @param tagName signature tag name.
 */
std::vector<unsigned char> digidoc::Signature::calcDigestOnNode(Digest* calc, const std::string& ns,
		const std::string& tagName) const throw(SignatureException)
{
	try
	{
		// Parse Xerces DOM from file, to preserve the white spaces "as is"
		// and get the same digest value on XML node.
		// Canonical XML 1.0 specification (http://www.w3.org/TR/2001/REC-xml-c14n-20010315)
		// needs all the white spaces from XML file "as is", otherwise the digests won't match.
		// Therefore we have to use Xerces to parse the XML file each time a digest needs to be
		// calculated on a XML node. If you are parsing XML files with a parser that doesn't
		// preserve the white spaces you are DOOMED!
	    std::auto_ptr<xercesc::DOMDocument> dom = createDom();

	    // Select node, on which the digest is calculated.
	    XMLCh* tagNs( xercesc::XMLString::transcode( ns.c_str() ) );
	    XMLCh* tag( xercesc::XMLString::transcode( tagName.c_str() ) );
        xercesc::DOMNodeList* nodeList = dom->getElementsByTagNameNS( tagNs, tag );
		xercesc::XMLString::release( &tagNs );
		xercesc::XMLString::release( &tag );

        // Make sure that exactly one node was found.
        if((nodeList == NULL) || (nodeList->getLength() < 1))
        {
            THROW_SIGNEXCEPTION("Could not find '%s' node which is in '%s' namespace in signature XML.", tagName.c_str(), ns.c_str());
        }

        if(nodeList->getLength() > 1)
        {
            THROW_SIGNEXCEPTION("Found %d '%s' nodes which are in '%s' namespace in signature XML, can not calculate digest on XML node.",
            		nodeList->getLength(), tagName.c_str(), ns.c_str());
        }


        // Canocalize XML using "http://www.w3.org/TR/2001/REC-xml-c14n-20010315" implementation.
	    XSECC14n20010315 canonicalizer(dom.get(), nodeList->item(0));
	    canonicalizer.setCommentsProcessing(false);
	    canonicalizer.setUseNamespaceStack(true);
	    canonicalizer.setExclusive();


	    //std::vector<unsigned char> c14n;
	    unsigned char buffer[1024];
	    int bytes = 0;
	    while((bytes = canonicalizer.outputBuffer(buffer, 1024)) > 0)
	    {
		    calc->update(buffer, bytes);
	        //for(int i = 0; i < bytes; i++) { c14n.push_back(buffer[i]); }
	    }
	    //DEBUG("c14n = '%s'", std::string(reinterpret_cast<char*>(&c14n[0]), 0, c14n.size()).c_str());

	    return calc->getDigest();
	}
	catch(const IOException& e)
	{
        THROW_SIGNATUREEXCEPTION_CAUSE(e, "Failed to create Xerces DOM from signature XML.");
	}
	return std::vector<unsigned char>();
}

/**
 * Parses XML from file to canonicalize a node and calculate digest on it.
 *
 * @return returns Xerces DOM document created from current signature
 */
 std::auto_ptr<xercesc::DOMDocument> digidoc::Signature::createDom() const throw(IOException)
{
    std::string path = this->path;

    // Save to file an parse it again, to make XML Canonicalization work
    // correctly as expected by the Canonical XML 1.0 specification.
    // Hope, the next Canonical XMl specification fixes the white spaces preserving "bug".
	if(isNew)
	{
	    path = util::File::tempFileName();
		saveToXml(path);
	}

    try
    {
    	// Initialize Xerces parser.
        std::auto_ptr<xercesc::XercesDOMParser> parser(new xercesc::XercesDOMParser());
        parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
        parser->setDoNamespaces(true);
        //xercesc::ErrorHandler* errorHandler = /*(xercesc::ErrorHandler*)*/ new xercesc::HandlerBase();
        //parser->setErrorHandler(errorHandler);

        // Parse and return a copy of the Xerces DOM tree.
        parser->parse(path.c_str());
        xercesc::DOMNode* dom = parser->getDocument()->cloneNode(true);
        return std::auto_ptr<xercesc::DOMDocument>(static_cast<xercesc::DOMDocument*>(dom));
    }
    catch( const xercesc::XMLException& e )
    {
		char* tmp = xercesc::XMLString::transcode( e.getMessage() );
        std::string msg( tmp );
		xercesc::XMLString::release( &tmp );
        THROW_IOEXCEPTION( "Failed to parse signature XML: %s", msg.c_str() );
    }
    catch( const xercesc::DOMException& e )
    {
		char* tmp = xercesc::XMLString::transcode( e.msg );
        std::string msg( tmp );
		xercesc::XMLString::release( &tmp );
        THROW_IOEXCEPTION( "Failed to parse signature XML: %s", msg.c_str() );
    }
    catch(...)
    {
        THROW_IOEXCEPTION("Failed to parse signature XML.");
    }
	return std::auto_ptr<xercesc::DOMDocument>(NULL);
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
    saveToXml(path);
    return path;
}

/**
 * Saves signature to file using XAdES XML format.
 *
 * @param path path, where the signature XML file is saved.
 * @throws IOException throws exception if the signature file creation failed.
 */
void digidoc::Signature::saveToXml(const std::string path) const throw(IOException)
{
    // Serialize XML to file.
    DEBUG("Serializing XML to '%s'", path.c_str());

    std::ofstream ofs(digidoc::util::File::fstreamName(path).c_str());
	try 
	{
		dsig::signature(ofs, *signature, createNamespaceMap());
	}
	catch ( xsd::cxx::xml::invalid_utf8_string )
	{
		THROW_IOEXCEPTION( "Failed to create signature XML file. Parameters must be in UTF-8." );
	}
    // ofs.close(); -=K=-: pointless close, destructor does it

    if(ofs.fail())
    {
        THROW_IOEXCEPTION("Failed to create signature XML file to '%s'.", path.c_str());
    }

    // TODO: remove printing to std out.
    INFO("------------------------------------------------------------------------------------------------------------");
    dsig::signature(std::cout, *signature, createNamespaceMap());
    INFO("------------------------------------------------------------------------------------------------------------");
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


/**
 * Signer certificate taken from current signature.
 *
 * @return returns the SignedSignaturePropertiesType object.
 * @throws SignatureException
*/
digidoc::X509Cert digidoc::Signature::getSigningCertificate() const throw(SignatureException)
{
    const dsig::X509DataType::X509CertificateType& certBlock = getSigningX509CertificateType();
    unsigned char * data = reinterpret_cast<unsigned char*>( const_cast<char*>( certBlock.data() ) );
    std::vector<unsigned char> x509Bytes(certBlock.size());
    std::copy(data, data+certBlock.size(), &x509Bytes[0]);
    try
    {
        X509Cert cert(x509Bytes);
        return cert;
    }
    catch( const IOException &e )
    {
        THROW_SIGNATUREEXCEPTION_CAUSE( e, "Failed to read X509 certificate" );
    }
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
