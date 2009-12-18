#pragma once

#include "ADoc.h"

namespace digidoc
{

/**
 * Implements wrapper signed digital document container.
 * Container can contain several files and all these files can be signed using
 * signing certificates. Container can only be signed if it contains documents.
 * Documents can be added and removed from container only if the container is
 * not signed. To add or remove documents from signed container remove all the
 * signatures before modifying documents list in container.
 */
class EXP_DIGIDOC WDoc: public ADoc
{
public:
	enum DocumentType
	{
		BDocType,
		DDocType,
	};

	WDoc();
	WDoc( DocumentType type );
	WDoc( ADoc *doc );
	WDoc(std::string path) throw(IOException, BDocException);
	WDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException);
	~WDoc();

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

	void setType( DocumentType type );
	DocumentType documentType() const;

	void getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException);

private:
	ADoc *m_doc;
	DocumentType m_type;
};

}
