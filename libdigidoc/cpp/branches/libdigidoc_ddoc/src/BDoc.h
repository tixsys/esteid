#if !defined(__BDOC_H_INCLUDED__)
#define __BDOC_H_INCLUDED__

#include <memory>
#include <string>
#include <vector>

#include "ADoc.h"
#include "BDocException.h"
#include "Document.h"
#include "Signature.h"
#include "io/IOException.h"
#include "io/ISerialize.h"
#include "crypto/signer/Signer.h"



namespace digidoc
{

    void initialize();
    void terminate();


    /**
     * Implements the BDOC specification of the signed digital document container.
     * Container can contain several files and all these files can be signed using
     * signing certificates. Container can only be signed if it contains documents.
     * Documents can be added and removed from container only if the container is
     * not signed. To add or remove documents from signed container remove all the
     * signatures before modifying documents list in container.
     *
     * @author Janari Põld
     */
    class BDoc: public ADoc
    {

      public:
          BDoc();
          BDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException);
          virtual ~BDoc();
          void save() throw(IOException, BDocException);
          void saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException);
          void addDocument(const Document& document) throw(BDocException);
          Document getDocument(unsigned int id) const throw(BDocException);
          void removeDocument(unsigned int id) throw(BDocException);
          unsigned int documentCount() const;
          const Signature* getSignature(unsigned int id) const throw(BDocException);
          void removeSignature(unsigned int id) throw(BDocException);
          unsigned int signatureCount() const;
          void sign(Signer* signer, Signature::Type profile = Signature::BES) throw(BDocException);

      protected:
          void readFrom(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException);
          void addSignature(Signature* signature) throw(BDocException);
          void setVersion(const std::string& version);
          std::string getMimeType() const;
          std::string createMimetype() throw(IOException);
          std::string createManifest() throw(IOException);
          void readMimetype(std::string path) throw(IOException, BDocException);
          void parseManifestAndLoadFiles(std::string path) throw(IOException, BDocException);
          void validateSignatures() throw(BDocException);

          std::auto_ptr<ISerialize> serializer;
          std::vector<Document> documents;
          std::vector<Signature*> signatures;

      private:
          static const std::string MIMETYPE_PREFIX;
          static const std::string MANIFEST_NAMESPACE;
          std::string version;

    };
}

#endif // !defined(__BDOC_H_INCLUDED__)
