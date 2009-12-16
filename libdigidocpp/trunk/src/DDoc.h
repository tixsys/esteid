#pragma once

#include "ADoc.h"

namespace digidoc
{

class DDocPrivate;

/**
 * DDoc Signature class
 */
class EXP_DIGIDOC SignatureDDOC: public Signature
{
public:
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
	SignatureDDOC( int id, DDocPrivate *doc );

	int m_id;
	DDocPrivate *m_doc;

	friend class DDocPrivate;
};

/**
 * Implements the DDOC specification of the signed digital document container.
 * Container can contain several files and all these files can be signed using
 * signing certificates. Container can only be signed if it contains documents.
 * Documents can be added and removed from container only if the container is
 * not signed. To add or remove documents from signed container remove all the
 * signatures before modifying documents list in container.
 */
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
