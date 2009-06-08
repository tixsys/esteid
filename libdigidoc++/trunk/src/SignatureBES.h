#if !defined(__SIGNATUREBES_H_INCLUDED__)
#define __SIGNATUREBES_H_INCLUDED__

#include <string>

#include "Signature.h"
#include "BDoc.h"

namespace digidoc
{
    class SignatureBES : public Signature
    {

      public:
          SignatureBES(BDoc& bdoc);
          SignatureBES(const std::string& path, BDoc& bdoc) throw(SignatureException);
          virtual std::string getMediaType() const;
          virtual void validateOffline() const throw(SignatureException);
          virtual OCSP::CertStatus validateOnline() const throw(SignatureException);

          static const std::string MEDIA_TYPE;

      protected:
          virtual void sign(Signer* signer) throw(SignatureException, SignException);

          std::string id() const throw(SignatureException);
          digidoc::dsig::KeyInfoType& keyInfo() const throw(SignatureException);

          BDoc& bdoc;

      private:
          SignatureBES& operator=( SignatureBES const& that ); // undefine default

          void checkIfWellFormed() throw(SignatureException);

          // offline checks
          void checkSignature() const throw(SignatureException);
          void checkSignedInfo() const throw(SignatureException);
          void checkCanonicalizationMethod() const throw(SignatureException);
          void checkSignatureMethod() const throw(SignatureException);
          void checkReferences() const throw(SignatureException);
          void checkSignatureValue() const throw(SignatureException);
		  void checkSigningCertificate() const throw(SignatureException);
          void checkKeyInfo() const throw(SignatureException); // RSAKeyValue and X509Certificate
          void checkQualifyingProperties() const throw(SignatureException);
          void checkSignedSignatureProperties() const throw(SignatureException);

          bool isReferenceToSigProps(const digidoc::dsig::ReferenceType& refType) const throw(SignatureException);
          void checkReferenceToSigProps(const digidoc::dsig::ReferenceType& refType) const throw(SignatureException);
          void checkReferencesToDocs(dsig::SignedInfoType::ReferenceSequence& refSeq) const throw(SignatureException);
          void checkDocumentRefDigest(Document& doc, const std::string& documentFileName, const dsig::ReferenceType& refType) const throw(SignatureException);

    };
}

#endif // !defined(__SIGNATUREBES_H_INCLUDED__)
