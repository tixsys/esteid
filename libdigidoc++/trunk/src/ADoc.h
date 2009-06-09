#pragma once

#include "BDocException.h"
#include "Document.h"
#include "Signature.h"
#include "io/IOException.h"
#include "io/ISerialize.h"

namespace digidoc
{

class EXP_DIGIDOC ADoc
{
public:
	virtual ~ADoc() {}
	virtual void save() throw(IOException, BDocException) = 0;
	virtual void saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException) = 0;
	virtual void addDocument(const Document& document) throw(BDocException) = 0;
	virtual Document getDocument(unsigned int id) const throw(BDocException) = 0;
	virtual void removeDocument(unsigned int id) throw(BDocException) = 0;
	virtual unsigned int documentCount() const = 0;
	virtual const Signature* getSignature(unsigned int id) const throw(BDocException) = 0;
	virtual void removeSignature(unsigned int id) throw(BDocException) = 0;
	virtual unsigned int signatureCount() const = 0;
	virtual void sign(Signer* signer, Signature::Type profile = Signature::BES) throw(BDocException) = 0;
};

}
