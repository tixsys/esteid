#if !defined(__SIGNATURE_H_INCLUDED__)
#define __SIGNATURE_H_INCLUDED__

#include <string>

#include "SignatureException.h"
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
    	  enum Type { BES };

          virtual ~Signature();
          virtual std::string getMediaType() const = 0;
          std::string saveToXml() throw(IOException);

          Signer::SignatureProductionPlace getProductionPlace() const;
          Signer::SignerRole getSignerRole() const;
          std::string getSigningTime() const;
          X509* getSigningCertificate() const;

      protected:
          Signature();
          Signature(const std::string& path);
          void addReference(const std::string& uri, int digestMethod, std::vector<unsigned char> digestValue) throw(SignatureException);
          void setSigningCertificate(X509* cert);
          void setSignatureProductionPlace(const Signer::SignatureProductionPlace& signatureProductionPlace);
          void setSignerRole(const Signer::SignerRole& signerRole);
          virtual void sign(Signer* signer) = 0;

          const xades::SignedSignaturePropertiesType& getSignedSignatureProperties() const throw(SignatureException);

    	  std::auto_ptr<dsig::SignatureType> signature;

      private:
    	  std::auto_ptr<dsig::SignatureType> createEmptySignature();

    	  /**Create Xades root element*/
    	  std::auto_ptr<xades::QualifyingPropertiesType>  createXAdES();
    	  std::auto_ptr<xades::UnsignedPropertiesType> createXAdESUnsignedProperties();


    	  digidoc::xades::CertIDListType * createXAdESCertIDListType(
    	          const std::string& digestMethodAlgorithmUri,
    	          const ::xml_schema::Base64Binary& digestValueB64,
    	          const std::string& X509IssuerName,
    	          long long issuerSerialNumber);

    	  std::auto_ptr<dsig::SignatureType> parseSignature(const std::string& path);

          static const std::string XADES_NAMESPACE;
          static const std::string DSIG_NAMESPACE;

    	  std::string path;

    };
}

#endif // !defined(__SIGNATURE_H_INCLUDED__)
