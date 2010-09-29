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

#if !defined(__SIGNATUREBES_H_INCLUDED__)
#define __SIGNATUREBES_H_INCLUDED__

#include "BDoc.h"

namespace digidoc
{
    class EXP_DIGIDOC SignatureBES : public Signature
    {

      public:
          SignatureBES(BDoc& bdoc);
          SignatureBES(const std::string& path, BDoc& bdoc) throw(SignatureException);
          virtual ~SignatureBES();
          virtual std::string getMediaType() const;
          virtual void validateOffline() const throw(SignatureException);
          virtual OCSP::CertStatus validateOnline() const throw(SignatureException);

          static const std::string MEDIA_TYPE;

      protected:
          virtual void sign(Signer* signer) throw(SignatureException, SignException);

          std::string id() const throw(SignatureException);
          digidoc::dsig::KeyInfoType& keyInfo() const throw(SignatureException);

          BDoc& bdoc;

      private:
          SignatureBES& operator=( SignatureBES const& that ); // undefine default

          void checkIfWellFormed() throw(SignatureException);

          // offline checks
          void checkSignature() const throw(SignatureException);
          void checkSignedInfo() const throw(SignatureException);
          void checkSignatureMethod() const throw(SignatureException);
          void checkReferences() const throw(SignatureException);
          void checkSignatureValue() const throw(SignatureException);
		  void checkSigningCertificate() const throw(SignatureException);
          void checkKeyInfo() const throw(SignatureException); // RSAKeyValue and X509Certificate
          void checkQualifyingProperties() const throw(SignatureException);
          void checkSignedSignatureProperties() const throw(SignatureException);

          bool isReferenceToSigProps(const digidoc::dsig::ReferenceType& refType) const throw(SignatureException);
          void checkReferenceToSigProps(const digidoc::dsig::ReferenceType& refType) const throw(SignatureException);
          void checkReferencesToDocs(dsig::SignedInfoType::ReferenceSequence& refSeq) const throw(SignatureException);
          void checkDocumentRefDigest(Document& doc, const std::string& documentFileName, const dsig::ReferenceType& refType) const throw(SignatureException);

    };
}

#endif // !defined(__SIGNATUREBES_H_INCLUDED__)
