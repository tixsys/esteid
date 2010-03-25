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

#include <xercesc/dom/DOM.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <iostream>
#include <list>

#include "log.h"
#include "Conf.h"
#include "Document.h"
#include "SignatureBES.h"
#include "crypto/OpenSSLHelpers.h"
#include "crypto/Digest.h"
#include "crypto/cert/X509CertStore.h"
#include "crypto/crypt/RSACrypt.h"
#include "util/DateTime.h"
#include "util/File.h"

/**
 * BES profile signature media type.
 */
const std::string digidoc::SignatureBES::MEDIA_TYPE = "signature/bdoc-1.0/BES";

/**
 * Creates an empty BDOC-BES signature with mandatory XML nodes.
 */
digidoc::SignatureBES::SignatureBES(BDoc& bdoc)
 : Signature()
 , bdoc(bdoc)
{
}

/**
 *
 * @param path
 * @throws SignatureException
 */
digidoc::SignatureBES::SignatureBES(const std::string& path, BDoc& bdoc) throw(SignatureException)
 : Signature(path)
 , bdoc(bdoc)
{
    checkIfWellFormed();
}

digidoc::SignatureBES::~SignatureBES() {}

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
        checkQualifyingProperties();
    }
    catch (digidoc::Exception& e)
    {
        resultException.addCause(e); // remember and proceed
    }

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
        checkSigningCertificate();
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
    X509* issuerCert = X509CertStore::getInstance()->getCert(*(cert.getIssuerNameAsn1()));
    X509_scope issuerCertScope(&issuerCert);
    if(issuerCert == NULL)
    {
        THROW_SIGNATUREEXCEPTION("Failed to load issuer certificate.");
    }

    Conf* conf = Conf::getInstance();

    // Get OCSP responder certificate.
    // FIXME: throws IOException, handle it
    Conf::OCSPConf ocspConf = conf->getOCSP(cert.getIssuerName());
    if(ocspConf.issuer.empty())
    {
        SignatureException e(__FILE__, __LINE__, "Failed to find ocsp responder.");
        e.setCode( Exception::OCSPResponderMissing );
        throw e;
    }
    STACK_OF(X509)* ocspCerts = X509Cert::loadX509Stack(ocspConf.cert);
    X509Stack_scope ocspCertsScope(&ocspCerts);

    // Check the certificate validity from OCSP server.
    OCSP ocsp(ocspConf.url);
    ocsp.setSkew(120);//XXX: load from conf
    ocsp.setOCSPCerts(ocspCerts);
    std::auto_ptr<Digest> calc = Digest::create();
    calc->update(getSignatureValue());
    try
    {
        return ocsp.checkCert(cert.getX509(), issuerCert, calc->getDigest());
    }
    catch(const IOException& e)
    {
        THROW_SIGNATUREEXCEPTION("Failed to check the certificate validity from OCSP server.");
    }
    catch(const OCSPException& e)
    {
        THROW_SIGNATUREEXCEPTION("Failed to check the certificate validity from OCSP server.");
    }
    return digidoc::OCSP::GOOD;
}

/**
 * Sign the signature using BDOC-BES profile. Sets required fields,
 * calculates digests and finally signs the signature object using
 * the provided <code>signer</code> implementation.
 *
 * @param signer signer that signs the signature object.
 * @throws SignatureException exception is throws if signing failed.
 */
void digidoc::SignatureBES::sign(Signer* signer) throw(SignatureException, SignException)
{
    // Set required signature fields.
    try
    {
        setSigningCertificate(signer->getCert());
        setSignatureProductionPlace(signer->getSignatureProductionPlace());
        setSignerRole(signer->getSignerRole());
        setSigningTime(util::date::currentTime());
    }
    catch( const IOException &e )
    {
        THROW_SIGNEXCEPTION_CAUSE( e, "Failed to sign document" );
    }

    // Calculate digest of the Signature->Object->SignedProperties node.
    std::auto_ptr<Digest> calc = Digest::create();
    std::vector<unsigned char> digest = calcDigestOnNode(calc.get(), XADES_NAMESPACE, "SignedProperties");
    addReference("#S0-SignedProperties", calc->getUri(), digest, "http://uri.etsi.org/01903#SignedProperties");

    // Calculate SHA1 digest of the Signature->SignedInfo node.
    calc = Digest::create(NID_sha1);
    std::vector<unsigned char> sha1 = calcDigestOnNode(calc.get(), DSIG_NAMESPACE, "SignedInfo");
    Signer::Digest sigDigestSha1 = { NID_sha1, &sha1[0], calc->getSize() };

    // Sign the calculated SAH1 digest and add the signature value (SHA1-RSA) to the signature.
    std::vector<unsigned char> buf(128);
    Signer::Signature signatureSha1Rsa = { &buf[0], buf.size() };
    signer->sign(sigDigestSha1, signatureSha1Rsa);
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
    checkSignedInfo();
    checkKeyInfo();
    checkSignatureValue();
}

/// validate SignedInfo offline
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignedInfo() const throw(SignatureException)
{
    checkSignatureMethod();
    checkReferences();
}

/// validate SignatureMethod offline
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignatureMethod() const throw(SignatureException)
{
    dsig::SignedInfoType& signedInfo = signature->signedInfo();
    dsig::SignatureMethodType& sigMethod = signedInfo.signatureMethod();
    dsig::SignatureMethodType::AlgorithmType& algorithmType = sigMethod.algorithm();
    if ( algorithmType != URI_ID_RSA_SHA1 )//FIXME: const or dynamic
    {
        THROW_SIGNATUREEXCEPTION("Unsupported SignedInfo signature method \"%s\"", algorithmType.c_str());
    }
}

/// Check that SignedInfo contains exactly one Reference to every document,
/// plus one Reference to the SignedProperties
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
void digidoc::SignatureBES::checkKeyInfo() const throw(SignatureException)
{
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

	if ( !Digest::isSupported(certDigestMethodAlgorithm) )
	{
        THROW_SIGNATUREEXCEPTION("Unsupported digest algorithm  %s for signing certificate", certDigestMethodAlgorithm.c_str());
    }

	dsig::X509IssuerSerialType::X509IssuerNameType certIssuerName = certs[0].issuerSerial().x509IssuerName();
	dsig::X509IssuerSerialType::X509SerialNumberType certSerialNumber = certs[0].issuerSerial().x509SerialNumber();

	try
	{
		if ( x509.compareIssuerToString(certIssuerName) || x509.getSerial() != certSerialNumber )
		{
			DEBUG("certIssuerName: \"%s\"", certIssuerName.c_str());
			DEBUG("x509.getCertIssuerName() \"%s\"", x509.getIssuerName().c_str());
			DEBUG("sertCerials = %ld %ld", x509.getSerial(), (long)certSerialNumber);
			THROW_SIGNATUREEXCEPTION("Signing certificate issuer information does not match");
		}
	}
	catch( const IOException &e )
	{
		THROW_SIGNATUREEXCEPTION_CAUSE(e, "Signing certificate issuer information not valid");
	}

	xades::DigestAlgAndValueType::DigestValueType const& certDigestValue = certs[0].certDigest().digestValue();

	std::auto_ptr<Digest> certDigestCalc = Digest::create(certDigestMethodAlgorithm);

	// lets check digest with x509 that was in keyInfo
    std::vector<unsigned char> derEncodedX509 = x509.encodeDER();
    certDigestCalc->update(&derEncodedX509[0], derEncodedX509.size());
	std::vector<unsigned char> calcDigest = certDigestCalc->getDigest();

	if ( certDigestValue.size() != static_cast<size_t>( certDigestCalc->getSize() ) )
    {
        THROW_SIGNATUREEXCEPTION("Wrong length for signing certificate digest");
    }

	for ( size_t i = 0; i < static_cast<size_t>( certDigestCalc->getSize() ); ++i )
	{
		if ( calcDigest[i] != static_cast<unsigned char>(certDigestValue.data()[i]) )
		{
		    DEBUGMEM("Document cert digest", &(certDigestValue.data())[0], certDigestValue.size());
		    DEBUGMEM("Calculated cert digest", &calcDigest[0], calcDigest.size());
	        THROW_SIGNATUREEXCEPTION("Signing certificate digest does not match");
		}
	}
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkQualifyingProperties() const throw(SignatureException)
{
	dsig::ObjectType::QualifyingPropertiesSequence const& qProps = signature->object()[0].qualifyingProperties();

	if ( qProps.size() != 1 )
    {
        THROW_SIGNATUREEXCEPTION("Number of QualifyingProperties is %d, must be 1", qProps.size());
    }

	if ( qProps[0].target() != "#" + signature->id().get() )
    {
        THROW_SIGNATUREEXCEPTION("QualifyingProperties target is not Signature");
    }

	checkSignedSignatureProperties();

	if ( qProps[0].unsignedProperties().present() )
	{
		xades::QualifyingPropertiesType::UnsignedPropertiesType uProps = qProps[0].unsignedProperties().get();
		if ( uProps.unsignedDataObjectProperties().present() )
		{
	        THROW_SIGNATUREEXCEPTION("unexpected UnsignedDataObjectProperties in Signature");
		}
		if ( !uProps.unsignedSignatureProperties().present() )
		{
	        THROW_SIGNATUREEXCEPTION("UnsignedProperties must contain UnsignedSignatureProperties");
		}
	}
}

/// TODO: comment
///
/// @throws SignatureException on a problem in signature
void digidoc::SignatureBES::checkSignedSignatureProperties() const throw(SignatureException)
{
	const xades::SignedSignaturePropertiesType& signedProps = getSignedSignatureProperties();
	xades::SignedSignaturePropertiesType::SignaturePolicyIdentifierOptional policyOpt = signedProps.signaturePolicyIdentifier();
	if ( policyOpt.present() )
	{
        THROW_SIGNATUREEXCEPTION("Signature policy is not valid");
	}

}

/// Check if signing certificate was issued by trusted party.
/// @throws SignatureException on a problem with signing certificate
void digidoc::SignatureBES::checkSigningCertificate() const throw(SignatureException)
{
    X509Cert signingCert = getSigningCertificate();

    bool valid = false;
    try
    {
        valid = signingCert.verify();
    }
    catch( const IOException &e )
    {
        THROW_SIGNATUREEXCEPTION_CAUSE( e, "Unable to verify signing certificate" );
    }
    if(!valid)
    {
        THROW_SIGNATUREEXCEPTION("Unable to verify signing certificate %s", signingCert.getSubject().c_str());
    }
}

/// Check SignedInfo->Reference Type attribute is "http://uri.etsi.org/01903#SignedProperties"
/// @param refType SignedInfo->Reference element to check
/// @return refType Type attribute is http://uri.etsi.org/01903#SignedProperties
/// @throws SignatureException on a problem in signature
bool digidoc::SignatureBES::isReferenceToSigProps(const digidoc::dsig::ReferenceType& refType) const throw(SignatureException)
{
    const dsig::ReferenceType::TypeOptional& typeOpt = refType.type();

    if ( typeOpt.present() )
    {
        std::string typeAttr = typeOpt.get();
        //BDOC-1.0 spec says that value must be "http://uri.etsi.org/01903#SignedProperties",
        //but Xades wants value in format        http://uri.etsi.org/01903/vX.Y.Z/#SignedProperties,
        //where  X.Y.Z is Xades version
        //Try to support all possible values

        if((typeAttr.find("http://uri.etsi.org/01903") == 0)
            && (typeAttr.rfind("#SignedProperties") == (typeAttr.length() - std::string("#SignedProperties").length())))
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

/*  This check is pointless. It might make sense to check the syntax of the
 *  URL, but this is better left for the resolvers to handle.
    std::string foundUri = uriOpt.get();
    std::string expectedUri =
        std::string("#") + id() + "-SignedProperties";

    if ( foundUri != expectedUri )
    {
        THROW_SIGNATUREEXCEPTION("SignedInfo reference to SignedProperties attribute 'URI' is invalid");
    }
*/

    // check DigestMethod
    const dsig::DigestMethodType& digestMethod = refType.digestMethod();
    const dsig::DigestMethodType::AlgorithmType& algorithm = digestMethod.algorithm();

    if ( !Digest::isSupported( algorithm ) )
    {
        THROW_SIGNATUREEXCEPTION("reference to SignedProperties digest method algorithm '%s' is not supported", algorithm.c_str());
    }

    // check DigestValue
    const dsig::DigestValueType& digestValue = refType.digestValue();


    // TODO: do it nicely.
    //xml_schema::dom::auto_ptr<xercesc::DOMDocument> dom = createDom();
    //xercesc::DOMNode* signedPropsNode = dom->getFirstChild()->getLastChild()->getFirstChild()->getFirstChild();

	// xercesc::DOMNode* idNode(NULL);
// FIXME: Äkki oleks parem kasutada olemasolevat signature puud, mitte Xercese oma?
//	if (!signedPropsNode->hasAttributes()
//	   || (idNode = signedPropsNode->getAttributes()->getNamedItem(xercesc::XMLString::transcode("Id"))) == NULL )
//    {
//        THROW_SIGNATUREEXCEPTION("SignedProperties does not have attribute 'Id'");
//    }

    std::auto_ptr<Digest> calc = Digest::create(refType.digestMethod().algorithm());
	//std::vector<unsigned char> calculatedDigestValue = calcDigestOnNode(calc.get(), signedPropsNode);
	std::vector<unsigned char> calculatedDigestValue = calcDigestOnNode(calc.get(), XADES_NAMESPACE, "SignedProperties");

	if ( digestValue.begin() + calculatedDigestValue.size() != digestValue.end() )
	{
		THROW_SIGNATUREEXCEPTION("SignedProperties digest lengths do not match");
	}

	for ( size_t i = 0; i < calculatedDigestValue.size(); i++ )
	{
		const char* dv = digestValue.begin() + i;
		if ( *dv != static_cast<char>(calculatedDigestValue[i]) )
		{
		    DEBUGMEM("Document digest:", &digestValue.data()[0], digestValue.size());
		    DEBUGMEM("Calculated digest:", &calculatedDigestValue[0], calculatedDigestValue.size());
			THROW_SIGNATUREEXCEPTION("SignedProperties digest values do not match");
		}
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
                if ( digidoc::util::String::toUriFormat(documentFileName) == docRefUri )
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

    if ( !Digest::isSupported(algorithmType) )
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

    std::vector<unsigned char> docDigestBuf = doc.calcDigest(docDigest.get());

    // get the claimed digest
    const dsig::DigestValueType& digestValueType = refType.digestValue();
    const unsigned char* refDigest = reinterpret_cast<const unsigned char* >(digestValueType.data());



    if ( docDigestBuf.size() != digestValueType.size()
	    || memcmp(&docDigestBuf[0], refDigest, docDigestBuf.size()) != 0 )
    {
        DEBUGMEM("Claimed digest", refDigest, digestValueType.size());
        DEBUGMEM("Calculated digest", &docDigestBuf[0], docDigestBuf.size());

        THROW_SIGNATUREEXCEPTION("Document '%s' digest does not match with digest in signature"
            , documentFileName.c_str());
    }

}

/**
 * Validate signature value.
 *
 * @throws throws exception if signature value did not match.
 */
void digidoc::SignatureBES::checkSignatureValue() const throw(SignatureException)
{
	DEBUG("SignatureBES::checkSignatureValue()");
	try
	{
	    // Initialize RSA crypter.
        X509* cert = getSigningCertificate().getX509(); X509_scope certScope(&cert);
        RSACrypt rsa(cert);

        // Calculate SHA1 digest of the Signature->SignedInfo node.
	    std::auto_ptr<Digest> calc = Digest::create(NID_sha1);
        std::vector<unsigned char> sha1 = calcDigestOnNode(calc.get(), DSIG_NAMESPACE, "SignedInfo");
        DEBUGMEM("Digest", &sha1[0], sha1.size());

        // Get signature value.
	    std::vector<unsigned char> signatureSha1Rsa = getSignatureValue();

	    // Verify signature value with public RSA key.
        bool valid = rsa.verify(calc->getMethod(), sha1, signatureSha1Rsa);

        // Check that signature matched.
        if(!valid)
        {
    	    THROW_SIGNATUREEXCEPTION("Signature is not valid.");
        }
	}
    catch(const IOException& e)
    {
	    THROW_SIGNATUREEXCEPTION_CAUSE(e, "Failed to validate signature.");
    }
}
