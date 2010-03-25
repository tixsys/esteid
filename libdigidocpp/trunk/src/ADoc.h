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

#include "BDocException.h"
#include "Signature.h"
#include "io/IOException.h"

namespace digidoc
{

EXP_DIGIDOC void initialize();
EXP_DIGIDOC void terminate();

class Document;
class ISerialize;

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
	virtual void getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException) = 0;
};

}
