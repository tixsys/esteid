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
	void saveTo(std::string path) throw(IOException, BDocException);
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
