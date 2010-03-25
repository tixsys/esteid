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

#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocGen.h>

#include "BDocException.h"
#include "Signature.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

typedef int (*sym_calculateDataFileSizeAndDigest)( SignedDoc*, const char*, const char*, int );
typedef void (*sym_cleanupConfigStore)( ConfigurationStore* );
typedef int (*sym_convertStringToTimestamp)( SignedDoc*, const char*, Timestamp* );
typedef int (*sym_createOrReplacePrivateConfigItem)(ConfigurationStore*, const char*, const char* );
typedef int (*sym_createSignedDoc)( SignedDoc*, const char*, const char* );
typedef int (*sym_DataFile_delete)( SignedDoc*, const char* );
typedef int (*sym_DataFile_new)( DataFile**, SignedDoc*, const char*, const char*,
	const char*, const char*, long, const byte*, int, const char*, const char* );
typedef int (*sym_ddocPrepareSignature)( SignedDoc*, SignatureInfo**, const char*, const char*,
	const char*, const char*, const char*, X509*, const char* );
typedef int (*sym_ddocSaxReadSignedDocFromFile)( SignedDoc**, const char*, int, int );
typedef X509* (*sym_ddocSigInfo_GetOCSPRespondersCert)( const SignatureInfo * );
typedef X509* (*sym_ddocSigInfo_GetSignersCert)( const SignatureInfo* );
typedef int (*sym_ddocSigInfo_SetSignatureValue)( SignatureInfo*, const char*, long );
typedef char* (*sym_getErrorString)( int );
typedef void (*sym_initDigiDocLib)();
typedef int (*sym_initConfigStore)( const char* );
typedef int (*sym_notarizeSignature)( SignedDoc*, SignatureInfo* );
typedef int (*sym_ddocSaxExtractDataFile)( SignedDoc*, const char*,
	const char*, const char*, const char* );
typedef int (*sym_SignatureInfo_delete)( SignedDoc*, const char* );
typedef int (*sym_SignedDoc_free)( SignedDoc* );
typedef int (*sym_SignedDoc_new)( SignedDoc**, const char*, const char* );
typedef int (*sym_verifySignatureAndNotary)( SignedDoc*, SignatureInfo*, const char* );

namespace digidoc
{

class DDocLibrary
{
public:
	DDocLibrary();
	~DDocLibrary();
	void *resolve( const char *symbol );
	bool isOpen() const { return h; }

private:
#ifdef WIN32
	HINSTANCE h;
#else
	void *h;
#endif
};

struct DocumentDDoc
{
	std::string filename, mime;
};

class SignatureDDOC;

class DDocPrivate
{
public:
	DDocPrivate();
	~DDocPrivate();

	bool loadSymbols();

	DDocLibrary lib;
	SignedDoc *doc;
	std::string filename;
	std::string tmpFolder;
	bool ready;

	void loadSignatures();
	std::vector<SignatureDDOC*> signatures;
	std::vector<DocumentDDoc> documents;

	template<typename T>
	void throwCodeError( int err, const std::string &msg, int line ) const;
	void throwDocOpenError( int line ) const throw(BDocException);
	template<typename T>
	void throwError( const std::string &msg, int line ) const;
	template<typename T>
	void throwError2( const std::string &msg, Exception::ExceptionCode code, int line ) const;
	void throwSignError( const char *id, int err, const std::string &msg, int line ) const throw(BDocException);

	sym_calculateDataFileSizeAndDigest f_calculateDataFileSizeAndDigest;
	sym_cleanupConfigStore		f_cleanupConfigStore;
	sym_convertStringToTimestamp f_convertStringToTimestamp;
	sym_createOrReplacePrivateConfigItem f_createOrReplacePrivateConfigItem;
	sym_createSignedDoc			f_createSignedDoc;
	sym_DataFile_delete			f_DataFile_delete;
	sym_DataFile_new			f_DataFile_new;
	sym_ddocPrepareSignature	f_ddocPrepareSignature;
	sym_ddocSaxReadSignedDocFromFile f_ddocSaxReadSignedDocFromFile;
	sym_ddocSigInfo_GetOCSPRespondersCert f_ddocSigInfo_GetOCSPRespondersCert;
	sym_ddocSigInfo_GetSignersCert f_ddocSigInfo_GetSignersCert;
	sym_ddocSigInfo_SetSignatureValue f_ddocSigInfo_SetSignatureValue;
	sym_getErrorString			f_getErrorString;
	sym_initDigiDocLib			f_initDigiDocLib;
	sym_initConfigStore			f_initConfigStore;
	sym_notarizeSignature		f_notarizeSignature;
	sym_ddocSaxExtractDataFile	f_ddocSaxExtractDataFile;
	sym_SignatureInfo_delete	f_SignatureInfo_delete;
	sym_SignedDoc_free			f_SignedDoc_free;
	sym_SignedDoc_new			f_SignedDoc_new;
	sym_verifySignatureAndNotary f_verifySignatureAndNotary;
};

}
