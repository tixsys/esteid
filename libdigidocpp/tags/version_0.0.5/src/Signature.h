#if !defined(__SIGNATURE_H_INCLUDED__)
#define __SIGNATURE_H_INCLUDED__

#include <string>

#include "SignatureException.h"
#include "crypto/Digest.h"
#include "crypto/cert/X509Cert.h"
#include "crypto/ocsp/OCSP.h"
#include "crypto/signer/Signer.h"
#include "io/IOException.h"
#include "xml/OpenDocument_manifest.hxx"
#include "xml/xmldsig-core-schema.hxx"

namespace digidoc
{
    class Signature
    {
      friend class BDoc;

      public:
          enum Type { BES, TM };

          virtual ~Signature();
          virtual std::string getMediaType() const = 0;
          virtual void validateOffline() const throw(SignatureException) = 0;
          virtual OCSP::CertStatus validateOnline() const throw(SignatureException) = 0;
          std::string saveToXml() throw(IOException);

          Signer::SignatureProductionPlace getProductionPlace() const;
          Signer::SignerRole getSignerRole() const;
          std::string getSigningTime() const;
          X509Cert getSigningCertificate() const;

      protected:
          Signature();
          Signature(const std::string& path);
          void addReference(const std::string& uri, const std::string& digestUri, std::vector<unsigned char> digestValue,
                  const std::string& type = "") throw(SignatureException);
          void setSigningCertificate(const X509Cert& cert);
          void setSignatureProductionPlace(const Signer::SignatureProductionPlace& signatureProductionPlace);
          void setSignerRole(const Signer::SignerRole& signerRole);
          void setSigningTime(const xml_schema::DateTime& signingTime);
          void setSignatureValue(Signer::Signature signatureValue);
          std::string getBase64SignatureValue() const;
          virtual void sign(Signer* signer) throw(SignatureException) = 0;
          xml_schema::dom::auto_ptr<xercesc::DOMDocument> createDom() const;
          static xml_schema::NamespaceInfomap createNamespaceMap();
          static std::vector<unsigned char> calcDigestOnNode(Digest* calc, xercesc::DOMNode* node);
          static std::vector<unsigned char> c14nNode(xercesc::DOMNode* node);
          std::vector<unsigned char> createNonce() const;

          xades::SignedSignaturePropertiesType& getSignedSignatureProperties() const throw(SignatureException);
          dsig::X509DataType::X509CertificateType& getSigningX509CertificateType() const throw(SignatureException);

          dsig::SignatureType* signature;

      private:
          dsig::SignatureType* createEmptySignature();
          dsig::SignatureType* parseSignature(const std::string& path);

          static const std::string XADES_NAMESPACE;
          static const std::string DSIG_NAMESPACE;

          std::string path;

    };
}

#endif // !defined(__SIGNATURE_H_INCLUDED__)
