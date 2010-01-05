#include "DDoc_p.h"
#include "DDoc.h"

#include "crypto/cert/X509Cert.h"
#include "crypto/signer/Signer.h"
#include "io/ISerialize.h"
#include "util/File.h"
#include "Conf.h"
#include "Document.h"

using namespace digidoc;

#ifndef LIBDIGIDOC_NAME
#  if defined(__APPLE__)
#    define LIBDIGIDOC_NAME "libdigidoc.dylib"
#  elif defined(_WIN32)
#    define LIBDIGIDOC_NAME "digidoc.dll"
#  else
#    define LIBDIGIDOC_NAME "libdigidoc.so.2"
#  endif
#endif

#ifdef _WIN32

#include "XmlConf.h"

DDocLibrary::DDocLibrary()
{ h = LoadLibrary( LIBDIGIDOC_NAME ); }

DDocLibrary::~DDocLibrary()
{ if( isOpen() ) FreeLibrary( h ); }

void* DDocLibrary::resolve( const char *symbol )
{ return isOpen() ? (void*)GetProcAddress( h, symbol ) : NULL; }

#else

DDocLibrary::DDocLibrary()
{ h = dlopen( LIBDIGIDOC_NAME, RTLD_LAZY ); }

DDocLibrary::~DDocLibrary()
{ if( isOpen() ) dlclose( h ); }

void* DDocLibrary::resolve( const char *symbol )
{ return isOpen() ? dlsym( h, symbol ) : NULL; }

#endif



DDocPrivate::DDocPrivate()
:	doc(0)
,	ready(false)
,	f_calculateDataFileSizeAndDigest(0)
,	f_cleanupConfigStore(0)
,	f_convertStringToTimestamp(0)
,	f_createOrReplacePrivateConfigItem(0)
,	f_createSignedDoc(0)
,	f_DataFile_delete(0)
,	f_DataFile_new(0)
,	f_ddocPrepareSignature(0)
,	f_ddocSaxReadSignedDocFromFile(0)
,	f_ddocSigInfo_GetOCSPRespondersCert(0)
,	f_ddocSigInfo_GetSignersCert(0)
,	f_ddocSigInfo_SetSignatureValue(0)
,	f_getErrorString(0)
,	f_initDigiDocLib(0)
,	f_initConfigStore(0)
,	f_notarizeSignature(0)
,	f_ddocSaxExtractDataFile(0)
,	f_SignatureInfo_delete(0)
,	f_SignedDoc_free(0)
,	f_SignedDoc_new(0)
,	f_verifySignatureAndNotary(0)
{
	if( !loadSymbols() )
		return;

	f_initDigiDocLib();

#ifdef WIN32
    f_initConfigStore( util::File::path(XmlConf::getDefaultConfDir(), "/digidoc.ini").c_str() );
#else
	f_initConfigStore( NULL );
#endif

	ready = true;
}

DDocPrivate::~DDocPrivate()
{
	for( std::vector<SignatureDDOC*>::const_iterator i = signatures.begin();
		i != signatures.end(); ++i )
		delete *i;
	if( !f_SignedDoc_free || !f_cleanupConfigStore )
		return;
	f_SignedDoc_free( doc );
	f_cleanupConfigStore( NULL );
}

void DDocPrivate::loadSignatures()
{
	for( std::vector<SignatureDDOC*>::const_iterator i = signatures.begin();
		i != signatures.end(); ++i )
		delete *i;
	signatures.clear();
	for( int i = 0; i < doc->nSignatures; ++i )
		signatures.push_back( new SignatureDDOC( i, this ) );
}

bool DDocPrivate::loadSymbols()
{
	return
	(f_calculateDataFileSizeAndDigest = (sym_calculateDataFileSizeAndDigest)lib.resolve("calculateDataFileSizeAndDigest")) &&
	(f_cleanupConfigStore = (sym_cleanupConfigStore)lib.resolve("cleanupConfigStore")) &&
	(f_convertStringToTimestamp = (sym_convertStringToTimestamp)lib.resolve("convertStringToTimestamp")) &&
	(f_createOrReplacePrivateConfigItem = (sym_createOrReplacePrivateConfigItem)lib.resolve("createOrReplacePrivateConfigItem")) &&
	(f_createSignedDoc = (sym_createSignedDoc)lib.resolve("createSignedDoc")) &&
	(f_DataFile_delete = (sym_DataFile_delete)lib.resolve("DataFile_delete")) &&
	(f_DataFile_new = (sym_DataFile_new)lib.resolve("DataFile_new")) &&
	(f_ddocPrepareSignature = (sym_ddocPrepareSignature)lib.resolve("ddocPrepareSignature")) &&
	(f_ddocSaxReadSignedDocFromFile = (sym_ddocSaxReadSignedDocFromFile)lib.resolve("ddocSaxReadSignedDocFromFile")) &&
	(f_ddocSigInfo_GetOCSPRespondersCert = (sym_ddocSigInfo_GetOCSPRespondersCert)lib.resolve("ddocSigInfo_GetOCSPRespondersCert")) &&
	(f_ddocSigInfo_GetSignersCert = (sym_ddocSigInfo_GetSignersCert)lib.resolve("ddocSigInfo_GetSignersCert")) &&
	(f_ddocSigInfo_SetSignatureValue = (sym_ddocSigInfo_SetSignatureValue)lib.resolve("ddocSigInfo_SetSignatureValue")) &&
	(f_getErrorString = (sym_getErrorString)lib.resolve("getErrorString")) &&
	(f_initDigiDocLib = (sym_initDigiDocLib)lib.resolve("initDigiDocLib")) &&
	(f_initConfigStore = (sym_initConfigStore)lib.resolve("initConfigStore")) &&
	(f_notarizeSignature = (sym_notarizeSignature)lib.resolve("notarizeSignature")) &&
	(f_ddocSaxExtractDataFile = (sym_ddocSaxExtractDataFile)lib.resolve("ddocSaxExtractDataFile")) &&
	(f_SignatureInfo_delete = (sym_SignatureInfo_delete)lib.resolve("SignatureInfo_delete")) &&
	(f_SignedDoc_free = (sym_SignedDoc_free)lib.resolve("SignedDoc_free")) &&
	(f_SignedDoc_new = (sym_SignedDoc_new)lib.resolve("SignedDoc_new")) &&
	(f_verifySignatureAndNotary = (sym_verifySignatureAndNotary)lib.resolve("verifySignatureAndNotary"));
}

template<typename T>
void DDocPrivate::throwCodeError( int err, const std::string &msg, int line ) const
{
	switch( err )
	{
	case ERR_OK: break;
	case ERR_PKCS_LOGIN:
		throwError2<T>( "PIN Incorrect", Exception::PINIncorrect, line );
	case ERR_OCSP_CERT_REVOKED:
		throwError2<T>( "Certificate status: revoked", Exception::CertificateRevoked, line );
	case ERR_OCSP_CERT_UNKNOWN:
		throwError2<T>( "Certificate status: unknown", Exception::CertificateUnknown, line );
	case ERR_OCSP_RESP_NOT_TRUSTED:
		throwError2<T>( "Failed to find ocsp responder.", Exception::OCSPResponderMissing, line );
	case ERR_OCSP_CERT_NOTFOUND:
		throwError2<T>( "OCSP certificate loading failed", Exception::OCSPCertMissing, line );
	case ERR_OCSP_UNAUTHORIZED:
		throwError2<T>( "Unauthorized OCSP request", Exception::OCSPRequestUnauthorized, line );
	case ERR_UNKNOWN_CA:
	{ \
		std::ostringstream s;
		s << msg << "\n (error: " << err;
		if( f_getErrorString )
			s << "; message: " << f_getErrorString( err );
		s << ")";
		throwError2<T>( s.str(), Exception::CertificateUnknown, line );
	}
	default:
	{
		std::ostringstream s;
		s << msg << "\n (error: " << err;
		if( f_getErrorString )
			s << "; message: " << f_getErrorString( err );
		s << ")";
		throwError<T>( s.str(), line );
	}
	}
}

void DDocPrivate::throwDocOpenError( int line ) const throw(BDocException)
{
	if( !ready )
		throwError<BDocException>( "DDoc library not loaded", line );
	if( !doc )
		throwError<BDocException>( "Document not open", line );
}

template<typename T>
void DDocPrivate::throwError( const std::string &msg, int line ) const
{ throw T( __FILE__, line, msg ); }

template<typename T>
void DDocPrivate::throwError2( const std::string &msg, Exception::ExceptionCode code, int line ) const
{ T e( __FILE__, line, msg ); e.setCode( code ); throw e; }

void DDocPrivate::throwSignError( const char *id, int err, const std::string &msg, int line ) const throw(BDocException)
{
	if( err != ERR_OK )
		f_SignatureInfo_delete( doc, id );
	throwCodeError<BDocException>( err, msg, line );
}



/**
 * DDoc profile signature media type.
 */
SignatureDDOC::SignatureDDOC( int id, DDocPrivate *doc )
:	m_id( id ), m_doc( doc )
{
	if( doc == NULL )
		throw SignatureException( __FILE__, __LINE__, "Null pointer in SignatureDDOC constructor" );

	SignatureInfo *sig = doc->doc->pSignatures[id];
	X509 *cert = doc->f_ddocSigInfo_GetSignersCert( sig );
	try { setSigningCertificate( cert ); }
	catch( const Exception & ) {}

	Signer::Signature s;
	s.signature = (unsigned char*)sig->pSigValue->mbufSignatureValue.pMem;
	s.length = sig->pSigValue->mbufSignatureValue.nLen;
	setSignatureValue( s );

	SignatureProductionPlace l;
	if( sig->sigProdPlace.szCity )
		l.city = sig->sigProdPlace.szCity;
	if( sig->sigProdPlace.szStateOrProvince )
		l.stateOrProvince = sig->sigProdPlace.szStateOrProvince;
	if( sig->sigProdPlace.szPostalCode )
		l.postalCode = sig->sigProdPlace.szPostalCode;
	if( sig->sigProdPlace.szCountryName )
		l.countryName = sig->sigProdPlace.szCountryName;
	setSignatureProductionPlace( l );

	SignerRole r;
	for( int i = 0; i < sig->signerRole.nClaimedRoles; ++i )
		r.claimedRoles.push_back( sig->signerRole.pClaimedRoles[i] );
	setSignerRole( r );

	Timestamp ts;
	doc->f_convertStringToTimestamp( m_doc->doc, sig->szTimeStamp, &ts );
	setSigningTime( xml_schema::DateTime( ts.year, ts.mon, ts.day, ts.hour, ts.min, ts.sec ) );
}

/**
 * @return returns signature mimetype.
 */
std::string SignatureDDOC::getMediaType() const
{
	std::ostringstream s;
	s << m_doc->doc->szFormat << "/" << m_doc->doc->szFormatVer;
	return s.str();
}

/**
 * @return returns OCSP certificate
 */
X509Cert SignatureDDOC::getOCSPCertificate() const
{
	try { return X509Cert( m_doc->f_ddocSigInfo_GetOCSPRespondersCert( m_doc->doc->pSignatures[m_id] ) ); }
	catch( const Exception & ) {}
	return X509Cert();
}

/**
 * @return returns OCSP timestamp
 */
std::string SignatureDDOC::getProducedAt() const
{
	NotaryInfo *n = m_doc->doc->pSignatures[m_id]->pNotary;
	std::string ret;
	if( n && n->timeProduced )
		ret = n->timeProduced;
	size_t pos = 0;
	while( (pos = ret.find( '.', pos )) != std::string::npos )
		ret.replace( pos, 1, "-" );
	return ret;
}

/**
 * @return returns OCSP responder ID
 */
std::string SignatureDDOC::getResponderID() const
{
	NotaryInfo *n = m_doc->doc->pSignatures[m_id]->pNotary;
	return n ? std::string( (const char*)n->mbufRespId.pMem, n->mbufRespId.nLen ) : std::string();
}

/**
 * @param data Revocation digest
 * @param digestMethodUri Revocation digest method URI
 */
void SignatureDDOC::getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri) const throw(SignatureException)
{
	NotaryInfo *n = m_doc->doc->pSignatures[m_id]->pNotary;
	if( !n )
		return;
	data.resize( n->mbufOcspDigest.nLen );
	std::copy( (unsigned char*)n->mbufOcspDigest.pMem,
		(unsigned char*)n->mbufOcspDigest.pMem + n->mbufOcspDigest.nLen, data.begin() );
	if( n->szDigestType )
		digestMethodUri = n->szDigestType;
}

/**
 * Do TM offline validations.
 * <ul>
 *   <li>Validate BES offline</li>
 *   <li>Check OCSP response (RevocationValues) was signed by trusted OCSP server</li>
 *   <li>Check that nonce field in OCSP response is same as CompleteRevocationRefs-&gt;DigestValue</li>
 *   <li>Recalculate hash of signature and compare with nonce</li>
 * </ul>
 * @throws SignatureException if signature is not valid
 */
void SignatureDDOC::validateOffline() const throw(SignatureException)
{
	int err = m_doc->f_verifySignatureAndNotary(
		m_doc->doc, m_doc->doc->pSignatures[m_id], m_doc->filename.c_str() );
	m_doc->throwCodeError<SignatureException>( err, "Failed to validate signature", __LINE__ );
}

/**
 *
 * return
 * @throws SignatureException
 */
OCSP::CertStatus SignatureDDOC::validateOnline() const throw(SignatureException)
{
	validateOffline();
	return OCSP::GOOD;
}

/**
 *
 * @param signer
 * @throws SignatureException
 */
void SignatureDDOC::sign(Signer* signer) throw(SignatureException, SignException) {}



/**
 * Initialize DDOC container.
 */
DDoc::DDoc()
:	d( new DDocPrivate )
{
	if( !d->ready )
		return;
	/*int err =*/ d->f_SignedDoc_new( &d->doc, "DIGIDOC-XML", "1.3" );
	//throwError( err, "Failed to create new document", __LINE__ );
}

/**
 * Releases resources.
 */
DDoc::~DDoc() { delete d; }

/**
 * Opens DDOC container from a file
 */
digidoc::DDoc::DDoc(std::string path) throw(IOException, BDocException)
 :	d( new DDocPrivate )
{
	if( !d->ready )
		d->throwError<BDocException>( "DDoc library not loaded", __LINE__ );

	d->filename = path;
	loadFile();
}

/**
 * Opens DDOC container using <code>serializer</code> implementation.
 *
 * @param serializer implementation of the serializer.
 * @throws IOException exception is thrown if reading data from the container failed.
 * @throws BDocException exception is thrown if the container is not valid.
 */
DDoc::DDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
:	d( new DDocPrivate )
{
	if( !d->ready )
		d->throwError<BDocException>( "DDoc library not loaded", __LINE__ );

	d->filename = serializer->getPath();
	loadFile();
}

/**
 * Parses file data
 */
void DDoc::loadFile()
{
	try
	{
		util::File::createDirectory( d->tmpFolder = util::File::tempDirectory() );
	}
	catch( const Exception & )
	{ d->throwError<BDocException>( "Failed to create temporary directory", __LINE__ ); }

	int err = d->f_ddocSaxReadSignedDocFromFile( &d->doc, d->filename.c_str(), 0, 0 );
	switch( err )
	{
	case ERR_OK:
	case ERR_OCSP_CERT_REVOKED:
	case ERR_OCSP_CERT_UNKNOWN: break;
	default:
		d->f_SignedDoc_free( d->doc );
		d->doc = NULL;
		d->throwCodeError<BDocException>( err, "Failed to open ddoc file", __LINE__ );
		return;
	}

	for( int i = 0; i < d->doc->nDataFiles; ++i)
	{
		DataFile *data = d->doc->pDataFiles[i];
		DocumentDDoc doc = { d->tmpFolder + data->szFileName, data->szMimeType };
		if ( !strcmp( data->szContentType, CONTENT_HASHCODE ) )
			continue;
		int err = d->f_ddocSaxExtractDataFile( d->doc, d->filename.c_str(),
			doc.filename.c_str(), data->szId, CHARSET_UTF_8 );
		if( err != ERR_OK )
		{
			if( d->doc )
				d->f_SignedDoc_free( d->doc );
			d->doc = NULL;
			d->throwCodeError<BDocException>( err, "Failed to exctract files", __LINE__ );
		}
		d->documents.push_back( doc );
	}

	d->loadSignatures();
}

/**
 * Adds document to the container. Documents can be removed from container only
 * after all signatures are removed.
 *
 * @param document a document, which is added to the container.
 * @throws BDocException exception is thrown if the document path is incorrect or document
 *         with same file name already exists. Also no document can be added if the
 *         container already has one or more signatures.
 */
void DDoc::addDocument( const Document &document ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	DataFile *data = 0;
	int err = d->f_DataFile_new( &data, d->doc, NULL, document.getPath().c_str(),
		CONTENT_EMBEDDED_BASE64, document.getMediaType().c_str(), 0, NULL, 0,
		NULL, CHARSET_UTF_8 );
	d->throwCodeError<BDocException>( err, "Failed to add file '" + document.getPath() + "'" , __LINE__ );

	err = d->f_calculateDataFileSizeAndDigest(
		d->doc, data->szId, document.getPath().c_str(), DIGEST_SHA1 );
	d->throwCodeError<BDocException>( err, "Failed calculate file digest and size", __LINE__ );
	DocumentDDoc doc = { document.getPath(), document.getMediaType() };
	d->documents.push_back( doc );
}

/**
 * @return returns number of documents in container.
 */
unsigned int DDoc::documentCount() const { return d->documents.size(); }

/**
 * Returns document referenced by document id.
 *
 * @param id document id.
 * @return returns document referenced by document id.
 * @throws BDocException throws exception if the document id is incorrect.
 */
Document DDoc::getDocument( unsigned int id ) const throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->documents.size() )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->documents.size() << " documents in container.";
		d->throwError<BDocException>( s.str(), __LINE__ );
	}

	return Document( d->documents[id].filename, d->documents[id].mime );
}

/**
 * Returns signature referenced by signature id.
 *
 * @param id signature id.
 * @return returns signature referenced by signature id.
 * @throws BDocException throws exception if the signature id is incorrect.
 */
const Signature* DDoc::getSignature( unsigned int id ) const throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->signatures.size() )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->signatures.size() << " signatures in container.";
		d->throwError<BDocException>( s.str(), __LINE__ );
	}

	return d->signatures[id];
}

/**
 * Removes document from container by document id. Documents can be
 * removed from container only after all signatures are removed.
 *
 * @param id document's id, which will be removed.
 * @throws BDocException throws exception if the document id is incorrect or there are
 *         one or more signatures.
 */
void DDoc::removeDocument( unsigned int id ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->documents.size() )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->documents.size() << " documents in container.";
		d->throwError<BDocException>( s.str(), __LINE__ );
	}
	if( !d->signatures.empty() )
	{
		d->throwError<BDocException>(
			"Can not remove document from container which has signatures, "
			"remove all signatures before removing document.", __LINE__ );
	}

	int err = d->f_DataFile_delete( d->doc, d->doc->pDataFiles[id]->szId );
	d->throwCodeError<BDocException>( err, "Failed to delete file", __LINE__ );
	d->documents.erase( d->documents.begin() + id );
}

/**
 * Removes signature from container by signature id.
 *
 * @param id signature's id, which will be removed.
 * @throws BDocException throws exception if the signature id is incorrect.
 */
void DDoc::removeSignature( unsigned int id ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->signatures.size() )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->signatures.size() << " signatures in container.";
		d->throwError<BDocException>( s.str(), __LINE__ );
	}

	int err = d->f_SignatureInfo_delete( d->doc, d->doc->pSignatures[id]->szId );
	d->throwCodeError<BDocException>( err, "Failed to remove signature", __LINE__ );
	d->loadSignatures();
}

/**
 * Saves the container using the <code>serializer</code> implementation provided in
 * <code>readFrom()</code> method.
 *
 * @throws IOException is thrown if there was a failure saving BDOC container. For example added
 *         document does not exist.
 * @throws BDocException is thrown if BDoc class is not correctly initialized.
 */
void DDoc::save() throw(IOException, BDocException)
{
	d->throwDocOpenError( __LINE__ );
	int err = d->f_createSignedDoc( d->doc, d->filename.c_str(), d->filename.c_str() );
	d->throwCodeError<BDocException>( err, "Failed to save document", __LINE__ );
}

/**
 * Saves the container using the <code>serializer</code> implementation provided.
 *
 * @param serializer serializer implementation, used to save data to BDOC container.
 * @throws IOException is thrown if there was a failure saving BDOC container. For example added
 *         document does not exist.
 * @throws BDocException is thrown if BDOC class is not correctly initialized.
 * @see save()
 */
void DDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	d->throwDocOpenError( __LINE__ );
	d->filename = serializer->getPath();
	save();
}

/**
 * Signs all documents in container.
 *
 * @param signer signer implementation.
 * @param profile signature profile (e.g. BES, TM).
 * @throws BDocException exception is throws if signing the BDCO container failed.
 */
void DDoc::sign( Signer *signer, Signature::Type type ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if ( type == Signature::MOBILE )
	{
		std::string file = signer->signaturePath();
		SignedDoc *sigDoc = 0;
		int err = d->f_ddocSaxReadSignedDocFromFile( &sigDoc, file.c_str(), 0, 0 );
		d->throwCodeError<BDocException>( err, "Failed to sign document", __LINE__ );

		SignatureInfo **signatures = (SignatureInfo**)realloc( d->doc->pSignatures,
			(d->doc->nSignatures + sigDoc->nSignatures) * sizeof(void *));
		if( !signatures )
		{
			d->f_SignedDoc_free( sigDoc );
			d->throwError<BDocException>( "Failed to sign document", __LINE__ );
		}

		d->doc->pSignatures = signatures;
		for( int i = 0; i < sigDoc->nSignatures; ++i )
		{
			d->doc->pSignatures[d->doc->nSignatures + i] = sigDoc->pSignatures[i]; // take ownership
			sigDoc->pSignatures[i] = 0;
			// from ddocReadNewSignaturesFromDdoc
			((char*)d->doc->pSignatures[d->doc->nSignatures + i]->pDocs[0]->szDigest)[0] = 0x0A;
		}
		d->doc->nSignatures += sigDoc->nSignatures;
		sigDoc->nSignatures = 0;

		d->f_SignedDoc_free( sigDoc );
		d->loadSignatures();
		return;
	}

	SignatureInfo *info = NULL;
	std::ostringstream role;
	SignerRole::TRoles r = signer->getSignerRole().claimedRoles;
	for( SignerRole::TRoles::const_iterator i = r.begin(); i != r.end(); ++i )
	{
		role << *i;
		if( i + 1 != r.end() )
			role << " / ";
	}
	SignatureProductionPlace l = signer->getSignatureProductionPlace();

	int err = d->f_ddocPrepareSignature( d->doc, &info, role.str().c_str(), l.city.c_str(), l.stateOrProvince.c_str(),
		l.postalCode.c_str(), l.countryName.c_str(), X509_dup( signer->getCert() ), NULL );
	d->throwSignError( info->szId, err, "Failed to sign document", __LINE__ );

	std::vector<unsigned char> buf1(128);
	Signer::Signature signatureSha1Rsa = { &buf1[0], buf1.size() };

	std::vector<unsigned char> buf2(
		(unsigned char*)info->pSigInfoRealDigest->mbufDigestValue.pMem,
		(unsigned char*)info->pSigInfoRealDigest->mbufDigestValue.pMem + info->pSigInfoRealDigest->mbufDigestValue.nLen );
	Signer::Digest digest = { NID_sha1, &buf2[0], buf2.size() };

	try
	{
		signer->sign( digest, signatureSha1Rsa );
	}
	catch( const SignException &e )
	{
		d->f_SignatureInfo_delete( d->doc, info->szId );
		throw BDocException( __FILE__, __LINE__, "Failed to sign document", e );
	}
	err = d->f_ddocSigInfo_SetSignatureValue( info, (const char*)signatureSha1Rsa.signature, signatureSha1Rsa.length );
	d->throwSignError( info->szId, err, "Failed to sign document", __LINE__ );

	std::string host = Conf::getInstance()->getProxyHost();
	std::string port = Conf::getInstance()->getProxyPort();
	if( !host.empty() )
	{
		d->f_createOrReplacePrivateConfigItem( NULL, "USE_PROXY", "true" );
		d->f_createOrReplacePrivateConfigItem( NULL, "DIGIDOC_PROXY_HOST", host.c_str() );
		d->f_createOrReplacePrivateConfigItem( NULL, "DIGIDOC_PROXY_PORT", port.c_str() );
	}
	else
		d->f_createOrReplacePrivateConfigItem( NULL, "USE_PROXY", "false" );

	std::string cert = Conf::getInstance()->getPKCS12Cert();
	std::string pass = Conf::getInstance()->getPKCS12Pass();
	if( !cert.empty() )
	{
		d->f_createOrReplacePrivateConfigItem( NULL, "SIGN_OCSP", "true" );
		d->f_createOrReplacePrivateConfigItem( NULL, "DIGIDOC_PKCS_FILE", cert.c_str() );
		d->f_createOrReplacePrivateConfigItem( NULL, "DIGIDOC_PKCS_PASSWD", pass.c_str() );
	}
	else
		d->f_createOrReplacePrivateConfigItem( NULL, "SIGN_OCSP", "false" );

	err = d->f_notarizeSignature( d->doc, info );
	d->throwSignError( info->szId, err, "Failed to sign document", __LINE__ );
	d->loadSignatures();
}

/**
 * @return returns number of signatures in container.
 */
unsigned int DDoc::signatureCount() const { return d->signatures.size(); }

/**
 * Returns file digest format
 */
void DDoc::getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= (unsigned int)d->doc->nDataFiles || !d->doc->pDataFiles[id] )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->doc->nDataFiles << " documents in container.";
		d->throwError<BDocException>( s.str(), __LINE__ );
	}

	memcpy( digest, d->doc->pDataFiles[id]->mbufDigest.pMem, d->doc->pDataFiles[id]->mbufDigest.nLen );
}
