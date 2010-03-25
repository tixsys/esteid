/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
