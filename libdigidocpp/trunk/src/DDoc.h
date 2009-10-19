#pragma once

#include "ADoc.h"

namespace digidoc
{

class DDocPrivate;

class EXP_DIGIDOC SignatureDDOC: public Signature
{
public:
	SignatureDDOC( int id, DDocPrivate *doc );

	virtual std::string getMediaType() const;
	X509Cert getOCSPCertificate() const;
	std::string getProducedAt() const;
	std::string getResponderID() const;
	void getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri) const throw(SignatureException);
	virtual void validateOffline() const throw(SignatureException);
	virtual OCSP::CertStatus validateOnline() const throw(SignatureException);

protected:
	virtual void sign(Signer* signer) throw(SignatureException, SignException);

private:
	void throwError( std::string msg, int err, int line ) const throw(SignatureException);

	int m_id;
	DDocPrivate *m_doc;
};

class EXP_DIGIDOC DDoc: public ADoc
{
public:
	DDoc();
	DDoc(std::string path) throw(IOException, BDocException);
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
	void getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException);

private:
	DDocPrivate *d;
	void loadFile();
};

}
