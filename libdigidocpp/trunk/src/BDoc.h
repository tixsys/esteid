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

#if !defined(__BDOC_H_INCLUDED__)
#define __BDOC_H_INCLUDED__

#include "ADoc.h"
#include "Document.h"
#include "io/ISerialize.h"

namespace digidoc
{

    /**
     * Implements the BDOC specification of the signed digital document container.
     * Container can contain several files and all these files can be signed using
     * signing certificates. Container can only be signed if it contains documents.
     * Documents can be added and removed from container only if the container is
     * not signed. To add or remove documents from signed container remove all the
     * signatures before modifying documents list in container.
     *
     * Note: If non-ascii characters are present in XML data, we depend on the LANG variable to be set properly
     * (see iconv --list for the list of supported encoding values for libiconv).
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC BDoc: public ADoc
    {

      public:
          BDoc();
          BDoc(std::string path) throw(IOException, BDocException);
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
          void sign(Signer* signer, Signature::Type profile = Signature::TM) throw(BDocException);
          void getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException) {}

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
