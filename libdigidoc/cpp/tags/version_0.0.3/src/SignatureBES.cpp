#include <xercesc/dom/DOM.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <iostream>
#include <list>

#include "log.h"
#include "Conf.h"
#include "SignatureBES.h"
#include "crypto/OpenSSLHelpers.h"
#include "crypto/Digest.h"
#include "crypto/cert/X509CertStore.h"
#include "util/DateTime.h"
#include "util/File.h"

/**
 * BES profile signature media type.
 */
const std::string digidoc::SignatureBES::MEDIA_TYPE = "signature/bdoc-1.0/BES";

/**
 * Creates an empty BDOC-BES signature with mandatory XML nodes.
 */
digidoc::SignatureBES::SignatureBES(BDoc& _bdoc)
 : Signature()
 , bdoc(_bdoc)
{
}

/**
 *
 * @param path
 * @throws SignatureException
 */
digidoc::SignatureBES::SignatureBES(const std::string& path, BDoc& _bdoc) throw(SignatureException)
 : Signature(path)
 , bdoc(_bdoc)
{
    checkIfWellFormed();
}

/**
 * @return returns signature mimetype.
 */
std::string digidoc::SignatureBES::getMediaType() const
{
    return MEDIA_TYPE;
}

std::string digidoc::SignatureBES::id() const throw(SignatureException)
{
    const dsig::SignatureType::IdOptional& idOptional = signature->id();
    if ( !idOptional.present() )
    {
        THROW_SIGNATUREEXCEPTION("Signature element mandatory attribute 'Id' is missing");
    }
    std::string idString = idOptional.get();
    if ( idString.empty() )
    {
        THROW_SIGNATUREEXCEPTION("Signature element mandatory attribute 'Id' is empty");
    }

    return idString;
}

digidoc::dsig::KeyInfoType& digidoc::SignatureBES::keyInfo() const throw(SignatureException)
{
    dsig::SignatureType::KeyInfoOptional& keyInfoOptional = signature->keyInfo();
    if ( !keyInfoOptional.present() )
    {
        THROW_SIGNATUREEXCEPTION("Signature mandatory element KeyInfo is missing");
    }

    return keyInfoOptional.get();
}


/**
 * Check if signature is valid according to BDoc-BES format. Performs
 * any off-line checks that prove mathematical correctness.
 * However, there is no warranty against if the signature has expired. On-line
 * validation should be performed to check for signature expiration.
 *
 * @throws SignatureException containing details on what's wrong in this signature.
*/
void digidoc::SignatureBES::validateOffline() const throw(SignatureException)
{
    // A "master" exception containing all problems (causes) with this signature.
    // It'll be only thrown in case we have a reason (cause).
    SignatureException resultException(__FILE__, __LINE__, "Signature is invalid");

    try
    {
        checkSignature();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignedInfo();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkCanonicalizationMethod();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignatureMethod();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkReferences();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignatureValue();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkKeyInfo();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkObject();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkQualifyingProperties();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignedProperties();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignedSignatureProperties();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSigningTime();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSigningCertificate();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignatureProductionPlace();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    try
    {
        checkSignerRole();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }
    try
    {
        checkUnsignedProperties();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

    // now check if we've gathered some problems with this signature
    if ( resultException.hasCause() )
    {
        throw resultException;
    }
    // else: this signature is fine
}

/**
 *
 * return
 * @throws SignatureException
 */
digidoc::OCSP::CertStatus digidoc::SignatureBES::validateOnline() const throw(SignatureException)
{
	// FIXME: Add exception handling.

	// Get signing signature.
	X509Cert cert = getSigningCertificate();

	// Get issuer certificate.
	X509* issuerCert = X509CertStore::getInstance()->getCert(*(cert.getCertIssuerNameAsn1()));
	X509_scope issuerCertScope(&issuerCert);
	if(issuerCert == NULL)
	{
		THROW_SIGNATUREEXCEPTION("Failed to load issuer certificate.")
	}

    Conf* conf = Conf::getInstance();

    // Get OCSP responder certificate.
    // FIXME: throws IOException, handle it
    STACK_OF(X509)* ocspCerts = X509Cert::loadX509Stack(conf->getOCSPCertPath());
    X509Stack_scope ocspCertsScope(&ocspCerts);

    // Check the certificate validity from OCSP server.
	OCSP ocsp(conf->getOCSPUrl());
	ocsp.setOCSPCerts(ocspCerts);
	// FIXME: checkCert() throws IOExceptionand OCSPException, catch and handle these
	return ocsp.checkCert(cert.getX509(), issuerCert, createNonce());
}

/**
 * Sign the signature using BDOC-BES profile. Sets required fields,
 * calculates digests and finally signs the signature object using
 * the provided <code>signer</code> implementation.
 *
 * @param signer signer that signs the signature object.
 */
void digidoc::SignatureBES::sign(Signer* signer)
{
	// Set required signature fields.
    setSigningCertificate(signer->getCert());
    setSignatureProductionPlace(signer->getSignatureProductionPlace());
    setSignerRole(signer->getSignerRole());



    xml_schema::DateTime now = util::date::currentTime();
    setSigningTime(now);

    // Calculate SHA1 digest of the Signature->Object->SignedProperties node.
    // TODO: get node by name or id instead?
    xml_schema::dom::auto_ptr<xercesc::DOMDocument> dom = createDom();
    xercesc::DOMNode* signedPropsNode = dom->getFirstChild()->getLastChild()->getFirstChild()->getFirstChild();
    addReference("#S0-SignedProperties", NID_sha1, calcSHA1OnNode(signedPropsNode), "http://uri.etsi.org/01903/#SignedProperties");

    // Calculate SHA1 digest of the Signature->SignedInfo node.
    // TODO: get node by name or id instead?
    std::vector<unsigned char> sha1 = calcSHA1OnNode(createDom()->getFirstChild()->getFirstChild());
    Signer::Digest digest = { NID_sha1, &sha1[0], SHA1Digest::DIGEST_SIZE };

    // Sign the calculated digest and add the signature value to the signature.
    std::vector<unsigned char> buf(128);
    Signer::Signature signatureSha1Rsa = { &buf[0], buf.size() };
    signer->sign(digest, signatureSha1Rsa);
    setSignatureValue(signatureSha1Rsa);
}

/**
*
* @param path
* @return
*/
void digidoc::SignatureBES::checkIfWellFormed() throw(SignatureException)
{
    // Base class has verified the signature to be valid according to XML-DSig.
    // Now perform additional checks here and throw if this signature is
    // ill-formed according to BDoc.

    Signature::getSignedSignatureProperties(); // assumed to throw in case this block doesn't exist

    Signature::getSigningX509CertificateType(); // assumed to throw in case this block doesn't exist

    id(); // must exist

    // TODO: implement more checks?
}

/// validate Signature element offline
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignature() const throw(SignatureException)
{
    // attribute Id checked already - would be ill-formed by BDoc-BES otherwise
    // TODO: check xmlns attribute?

    checkSignedInfo();

    checkSignatureValue();

    checkKeyInfo();

    checkObject();
}

/// validate SignedInfo offline
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignedInfo() const throw(SignatureException)
{
    // existence checked by DSig already - would be ill-formed otherwise
    // TODO: check xmlns attribute?

    checkCanonicalizationMethod();

    checkSignatureMethod();

    checkReferences();
}

/// validate CanonicalizationMethod offline
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkCanonicalizationMethod() const throw(SignatureException)
{
    dsig::SignedInfoType& signedInfo = signature->signedInfo();
    dsig::CanonicalizationMethodType& canonMethod = signedInfo.canonicalizationMethod();
    dsig::CanonicalizationMethodType::AlgorithmType& algorithmType = canonMethod.algorithm();
    if ( algorithmType != "http://www.w3.org/TR/2001/REC-xml-c14n-20010315" )
    {
        THROW_SIGNATUREEXCEPTION("SignedInfo canonicalization method algorithm is wrong");
    }
}

/// validate SignatureMethod offline
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignatureMethod() const throw(SignatureException)
{
    dsig::SignedInfoType& signedInfo = signature->signedInfo();
    dsig::SignatureMethodType& sigMethod = signedInfo.signatureMethod();
    dsig::SignatureMethodType::AlgorithmType& algorithmType = sigMethod.algorithm();
    if ( algorithmType != "http://www.w3.org/2000/09/xmldsig#rsasha1" )
    {
        THROW_SIGNATUREEXCEPTION("SignedInfo signature method algorithm is wrong");
    }
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkReferences() const throw(SignatureException)
{
    dsig::SignedInfoType& signedInfo = signature->signedInfo();
    dsig::SignedInfoType::ReferenceSequence& refSeq = signedInfo.reference();

    if ( refSeq.size() != (bdoc.documentCount() + 1) )
    {
        // we require exactly one ref to every document, plus one ref to the SignedProperties
        THROW_SIGNATUREEXCEPTION("Number of references in SignedInfo is invalid: found %d, expected %d"
            , refSeq.size(), bdoc.documentCount() + 1);

    }

    // check reference to SignedProperties
    bool gotSignatureRef = false; // remember to ensure, it exists only once
    for ( dsig::SignedInfoType::ReferenceSequence::const_iterator itRef = refSeq.begin()
        ; itRef != refSeq.end()
        ; itRef++
        )
    {
        const dsig::ReferenceType& refType = (*itRef);

        if ( isReferenceToSigProps(refType) )
        {
            // the one and only reference to SignedProperties
            if ( gotSignatureRef )
            {
                THROW_SIGNATUREEXCEPTION("SignedInfo element refers to more than one SignedProperties");
            }
            gotSignatureRef = true; // remember this, we don't expect any more of those

            checkReferenceToSigProps(refType);
        } // else: skip, checked elsewhere
    }

    if ( !gotSignatureRef )
    {
        THROW_SIGNATUREEXCEPTION("SignedInfo does not contain reference to SignedProperties");
    }

    // check refs to documents
    checkReferencesToDocs(refSeq);

}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignatureValue() const throw(SignatureException)
{
    // existence checked by DSig already - would be ill-formed otherwise
    //const dsig::SignatureValueType& signatureValueType = signature->signatureValue();
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkKeyInfo() const throw(SignatureException)
{
    //dsig::KeyInfoType& keyInf = keyInfo(); // throws if missing

    // TODO
    // RSAKeyValue and X509Certificate

    // TODO: is this actually optional by BDoc?
    // Should we compare it to actual cert contents IF it is present (e.g. created by another lib).
    //<KeyValue>
    //    <RSAKeyValue>
    //    <Modulus>uVyoN5mbzWAKNOKIvXp1lnMEWThhf/gVblyjwiTquaAhyGeVGO7A+u0F4HZtjCKj6P0TxEtTvwQZPuPvM8aaMg3QXmSo+owDj7johtTZIaQ7oqCxYTIo7gLhC7gS/yaOBf288Vy0CuEr5Da8bWvZRwVDmaDRmNDZsN6dnvK2mbM=</Modulus>
    //    <Exponent>ngqL</Exponent>
    //    </RSAKeyValue>
    //</KeyValue>

	// will throw if there are no key info
	X509Cert x509 = getSigningCertificate();

    dsig::SignatureType::ObjectSequence const& objs = signature->object();

	if ( objs.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Number of Objects is %d, must be 1", objs.size());
    }

	dsig::ObjectType::QualifyingPropertiesSequence const& qProps = objs[0].qualifyingProperties();

	if ( qProps.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Number of QualifyingProperties is %d, must be 1", qProps.size());
    }

	xades::QualifyingPropertiesType::SignedPropertiesOptional const& sigProps =  qProps[0].signedProperties();

	if ( !sigProps.present() )
    {
        THROW_SIGNATUREEXCEPTION("SignedProperties not found");
    }

	xades::SignedSignaturePropertiesType::SigningCertificateOptional const& sigCertOpt = sigProps->signedSignatureProperties().signingCertificate();

	if ( !sigCertOpt.present() )
    {
        THROW_SIGNATUREEXCEPTION("SigningCertificate not found");
    }

	xades::CertIDListType::CertSequence const& certs = sigCertOpt->cert();

	if ( certs.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Number of SigningCertificates is %d, must be 1", certs.size());
    }

	dsig::DigestMethodType::AlgorithmType const& certDigestMethodAlgorithm = certs[0].certDigest().digestMethod().algorithm();
	if ( certDigestMethodAlgorithm != xml_schema::Uri("http://www.w3.org/2000/09/xmldsig#sha1") )
	{
        THROW_SIGNATUREEXCEPTION("Unsupported digest algorithm for signing certificate");
    }

	dsig::X509IssuerSerialType::X509IssuerNameType certIssuerName = certs[0].issuerSerial().x509IssuerName();
	dsig::X509IssuerSerialType::X509SerialNumberType certSerialNumber = certs[0].issuerSerial().x509SerialNumber();

	if ( certIssuerName != x509.getCertIssuerName() || x509.getCertSerial() != certSerialNumber )
	{
	        THROW_SIGNATUREEXCEPTION("Signing certificate issuer information does not match");
	}

	xades::DigestAlgAndValueType::DigestValueType const& digest = certs[0].certDigest().digestValue();

	if ( digest.size() != SHA1Digest::DIGEST_SIZE )
	{
        THROW_SIGNATUREEXCEPTION("Wrong length for signing certificate digest");
	}

	// lets check digest with x509 that was in keyInfo
    std::vector<unsigned char> derEncodedX509 = x509.encodeDER();
    SHA1Digest certSha1;
    certSha1.update(&derEncodedX509[0], derEncodedX509.size());
	std::vector<unsigned char> calcDigest = certSha1.getDigest();

	for ( size_t i = 0; i < SHA1Digest::DIGEST_SIZE; ++i )
	{
		if ( calcDigest[i] != static_cast<unsigned char>(digest.data()[i]) )
		{
	        THROW_SIGNATUREEXCEPTION("Signing certificate digest does not match");
		}
	}
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkObject() const throw(SignatureException)
{
    //const dsig::SignatureType::ObjectSequence& objectSequence = signature->object();
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkQualifyingProperties() const throw(SignatureException)
{
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignedProperties() const throw(SignatureException)
{
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignedSignatureProperties() const throw(SignatureException)
{
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSigningTime() const throw(SignatureException)
{
    // TODO
    // TODO: done in well-formed check?
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSigningCertificate() const throw(SignatureException)
{
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignatureProductionPlace() const throw(SignatureException)
{
    // TODO
    // TODO: done in well-formed check?
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignerRole() const throw(SignatureException)
{
    // TODO
    // TODO: done in well-formed check?
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkUnsignedProperties() const throw(SignatureException)
{
    // TODO
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
bool digidoc::SignatureBES::isReferenceToSigProps(const digidoc::dsig::ReferenceType& refType)
const throw(SignatureException)
{
    const dsig::ReferenceType::TypeOptional& typeOpt = refType.type();

    if ( typeOpt.present() )
    {
        std::string typeAttr = typeOpt.get();
        if ( typeAttr == "http://uri.etsi.org/01903/#SignedProperties" )
        {
            return true;
        }
    }

    return false;
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkReferenceToSigProps(const digidoc::dsig::ReferenceType& refType)
const throw(SignatureException)
{
    // check attribute URI (e.g. "#SigId-SignedProperties")
    const dsig::ReferenceType::URIOptional& uriOpt = refType.uRI();

    if ( !uriOpt.present() )
    {
        THROW_SIGNATUREEXCEPTION("SignedInfo reference to SignedProperties does not have attribute 'URI'");
    }

    std::string foundUri = uriOpt.get();
    std::string expectedUri =
        std::string("#") + id() + "-SignedProperties";

    if ( foundUri != expectedUri )
    {
        THROW_SIGNATUREEXCEPTION("SignedInfo reference to SignedProperties attribute 'URI' is invalid");
    }

    // check DigestMethod
    const dsig::DigestMethodType& digestMethod = refType.digestMethod();
    const dsig::DigestMethodType::AlgorithmType& algorithm = digestMethod.algorithm();
    if ( !isSupportedDigestAlgorithm(algorithm) )
    {
        THROW_SIGNATUREEXCEPTION("reference to SignedProperties digest method algorithm '%s' is not supported", algorithm.c_str());
    }

    // check DigestValue
    const dsig::DigestValueType& digestValue = refType.digestValue();

    // TODO: do it nicely.
    xml_schema::dom::auto_ptr<xercesc::DOMDocument> dom = createDom();
    xercesc::DOMNode* signedPropsNode = dom->getFirstChild()->getLastChild()->getFirstChild()->getFirstChild();

	xercesc::DOMNode* idNode(NULL);

	if (!signedPropsNode->hasAttributes()
	   || (idNode = signedPropsNode->getAttributes()->getNamedItem(xercesc::XMLString::transcode("Id"))) == NULL )
    {
        THROW_SIGNATUREEXCEPTION("SignedProperties does not have attribute 'Id'");
    }

	std::vector<unsigned char> calculatedDigestValue = calcSHA1OnNode(signedPropsNode);

	if ( digestValue.begin() + calculatedDigestValue.size() != digestValue.end() )
	{
		THROW_SIGNATUREEXCEPTION("SignedProperties digest lengths do not match");
	}

	for ( size_t i = 0; i < calculatedDigestValue.size(); i++ )
	{
		const char* dv = digestValue.begin() + i;
		if ( *dv != static_cast<char>(calculatedDigestValue[i]) )
		{
			THROW_SIGNATUREEXCEPTION("SignedProperties digest values do not match");
		}
	}
}

/**
 * @return true if we support given algorithm in BDoc-BES implementation
 */
bool digidoc::SignatureBES::isSupportedDigestAlgorithm(const std::string& algorithm) const
{
    // TODO:XXX: Simple-impl. Can this BDoc-BES list be different from what Digest class supports?
    // If yes, then freeze the list here.
    try
    {
    	Digest::toMethod(algorithm);
        return true;
    }
    catch (IOException&)
    {
        return false;
    }
}

// FIXME: this method does not work.
/**
 * Offline check on every SignedInfo reference to a document.
 * @throws SignatureException on error in references
 */
void digidoc::SignatureBES::checkReferencesToDocs(dsig::SignedInfoType::ReferenceSequence& refSeq)
const throw(SignatureException)
{
    // copy documents to a list for removal on every successful find
    std::vector< size_t > docNumberList;
    size_t docCount = bdoc.documentCount();
    for ( size_t i = 0; i != docCount; i++ )
    {
        docNumberList.push_back( i );
    }

    // loop over references
    for ( dsig::SignedInfoType::ReferenceSequence::const_iterator itRef = refSeq.begin()
        ; itRef != refSeq.end()
        ; itRef++
        )
    {
        const dsig::ReferenceType& refType = (*itRef);

        if ( !isReferenceToSigProps(refType) )
        {

            // get document URI
            const dsig::ReferenceType::URIOptional& uriOpt = refType.uRI();
            if ( !uriOpt.present() )
            {
                THROW_SIGNATUREEXCEPTION("Document reference is missing attribute 'URI'");
            }
            std::string docRefUri(uriOpt.get());

			// file names in manifest do not have '/' at front
			if ( !docRefUri.empty() && docRefUri[0] == '/' )
			{
				docRefUri.erase( 0, 1 );
			}

            // find the matching document from container
            bool foundDoc = false;
            for ( std::vector< size_t >::iterator itDocs = docNumberList.begin()
                ; itDocs != docNumberList.end(); itDocs++ )
            {
                Document doc = bdoc.getDocument( *itDocs );

                std::string documentFileName = util::File::fileName( doc.getPath() );

                if ( documentFileName == docRefUri )
                {
                    foundDoc = true;
                    docNumberList.erase( itDocs ); // remove from list to detect duplicate refs
                    checkDocumentRefDigest( doc, documentFileName, refType );
					break; // we modified docNumberList so get out of here
                }

            }

            if ( !foundDoc )
            {
                THROW_SIGNATUREEXCEPTION("Referenced Document '%s' not found in BDoc container", docRefUri.c_str());
            }

        } // else: skip, checked elsewhere
    }

    if ( !docNumberList.empty() )
    {
        // that's actually coder's bug - fix it
        THROW_SIGNATUREEXCEPTION("BDoc document list does not match the references block in signature");
    }
}

/**
 * Check if document digest matches with what reference claims
 */
void digidoc::SignatureBES::checkDocumentRefDigest(Document& doc, const std::string& documentFileName, const digidoc::dsig::ReferenceType& refType)
const throw(SignatureException)
{
    // get digest of the referred document
    const dsig::DigestMethodType& digestMethod = refType.digestMethod();
    const dsig::DigestMethodType::AlgorithmType& algorithmType = digestMethod.algorithm();

    if ( !isSupportedDigestAlgorithm(algorithmType) )
    {
        THROW_SIGNATUREEXCEPTION("Document reference '%s' has an unsupported digest algorithm '%s'"
            , documentFileName.c_str(), algorithmType.c_str());
    }

    std::auto_ptr< Digest > docDigest; // only this scope
    try
    {
        docDigest = Digest::create(algorithmType);
    }
    catch (IOException&)
    {
        // according to Digest contract:
        THROW_SIGNATUREEXCEPTION("Document reference '%s' algorithm '%s' is not implemented"
            , documentFileName.c_str(), algorithmType.c_str());
    }

    std::vector<unsigned char> docDigestBuf = doc.calcDigest(*docDigest);

    // get the claimed digest
    const dsig::DigestValueType& digestValueType = refType.digestValue();
    const unsigned char* refDigest = reinterpret_cast<const unsigned char* >(digestValueType.data());

    if ( docDigestBuf.size() != digestValueType.size()
	    || memcmp(&docDigestBuf[0], refDigest, docDigestBuf.size()) != 0 )
    {
        THROW_SIGNATUREEXCEPTION("Document '%s' digest does not match with digest in signature"
            , documentFileName.c_str());
    }

}
