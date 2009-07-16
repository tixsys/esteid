#pragma once

#include "ADoc.h"

namespace digidoc
{

class DDocPrivate;

class DDoc: public ADoc
{
public:
	DDoc();
	DDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException);
	virtual ~DDoc();

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

private:
	void throwError( const std::string &msg, int line ) const throw(BDocException);
	void throwError( int err, const std::string &msg, int line ) const throw(BDocException);
	void throwSignError( const char *id, int err, const std::string &msg, int line ) const throw(BDocException);

	DDocPrivate *d;
};

}
