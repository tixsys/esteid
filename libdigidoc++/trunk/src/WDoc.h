#pragma once

#include "ADoc.h"

namespace digidoc
{

class WDoc: public ADoc
{
public:
	enum DocumentType
	{
		BDocType,
		DDocType,
		CustomType,
	};

	WDoc();
	WDoc( DocumentType type );
	WDoc( ADoc *doc );
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

	DocumentType type() const;
	void setType( DocumentType type );

private:
	ADoc *m_doc;
	DocumentType m_type;
};

}
