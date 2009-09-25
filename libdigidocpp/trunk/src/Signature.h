#if !defined(__SIGNATURE_H_INCLUDED__)
#define __SIGNATURE_H_INCLUDED__

#include "SignatureException.h"
#include "crypto/ocsp/OCSP.h"
#include "crypto/signer/Signer.h"
#include "io/IOException.h"
#include "xml/xmldsig-core-schema.hxx"

namespace digidoc
{
    class Digest;
    class X509Cert;
    class EXP_DIGIDOC Signature
    {
      friend class BDoc;

      public:
          enum Type { BES, TM, MOBILE };

          virtual ~Signature();
          virtual std::string getMediaType() const = 0;
          virtual void validateOffline() const throw(SignatureException) = 0;
          virtual OCSP::CertStatus validateOnline() const throw(SignatureException) = 0;
          std::string saveToXml() throw(IOException);

          Signer::SignatureProductionPlace getProductionPlace() const;
          Signer::SignerRole getSignerRole() const;
          std::string getSigningTime() const;
          X509Cert getSigningCertificate() const throw(SignatureException);
          std::vector<unsigned char> getSignatureValue() const;

          virtual void getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri) const throw(SignatureException) = 0;

      protected:
          Signature();
          Signature(const std::string& path) throw(SignatureException);
          virtual void sign(Signer* signer) throw(SignatureException, SignException) = 0;

          void addReference(const std::string& uri, const std::string& digestUri, std::vector<unsigned char> digestValue,
                  const std::string& type = "") throw(SignatureException);
          void setSigningCertificate(const X509Cert& cert);
          void setSignatureProductionPlace(const Signer::SignatureProductionPlace& signatureProductionPlace);
          void setSignerRole(const Signer::SignerRole& signerRole);
          void setSigningTime(const xml_schema::DateTime& signingTime);
          void setSignatureValue(Signer::Signature signatureValue);

          xades::SignedSignaturePropertiesType& getSignedSignatureProperties() const throw(SignatureException);
          dsig::X509DataType::X509CertificateType& getSigningX509CertificateType() const throw(SignatureException);

          static xml_schema::NamespaceInfomap createNamespaceMap();
          std::vector<unsigned char> calcDigestOnNode(Digest* calc, const std::string& ns, const std::string& tagName)
                  const throw(SignatureException);

          dsig::SignatureType* signature;

          static const std::string XADES_NAMESPACE;
          static const std::string DSIG_NAMESPACE;

      private:
          dsig::SignatureType* createEmptySignature();
          dsig::SignatureType* parseSignature(const std::string& path) throw(IOException);
          std::auto_ptr<xercesc::DOMDocument> createDom() const throw(IOException);
          void saveToXml(const std::string path) const throw(IOException);

          std::string path;
          bool isNew;

    };
}

#endif // !defined(__SIGNATURE_H_INCLUDED__)
