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
	std::string conf;
	const char *bdoc = getenv( XmlConf::CONF_ENV.c_str() );
	if( bdoc )
	{
		conf.append( util::File::directory( bdoc ) );
		conf.append( "/" );
	}
	conf.append( "digidoc.ini" );
	f_initConfigStore( conf.c_str() );
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

void DDocPrivate::throwDocOpenError( int line ) const throw(BDocException)
{
	if( !ready )
		throwError( "DDoc library not loaded", line );
	if( !doc )
		throwError( "Document not open", line );
}

void DDocPrivate::throwError( const std::string &msg, int line ) const throw(BDocException)
{ throw BDocException( __FILE__, line, msg ); }

void DDocPrivate::throwError( int err, const std::string &msg, int line ) const throw(BDocException)
{
	switch( err )
	{
	case ERR_OK: break;
	case ERR_PKCS_LOGIN:
	{
		BDocException e( __FILE__, line, "PIN Incorrect" );
		e.setCode( Exception::PINIncorrect );
		throw e;
		break;
	}
	case ERR_OCSP_CERT_REVOKED:
	{
		BDocException e( __FILE__, line, "Certificate status: revoked" );
		e.setCode( Exception::CertificateRevoked );
		throw e;
		break;
	}
	case ERR_OCSP_CERT_UNKNOWN:
	{
		BDocException e( __FILE__, line, "Certificate status: unknown" );
		e.setCode( Exception::CertificateUnknown );
		throw e;
		break;
	}
	case ERR_OCSP_RESP_NOT_TRUSTED:
	{
		BDocException e( __FILE__, line, "Failed to find ocsp responder." );
		e.setCode( Exception::OCSPResponderMissing );
		throw e;
		break;
	}
	case ERR_OCSP_CERT_NOTFOUND:
	{
		BDocException e( __FILE__, line, "OCSP certificate loading failed");
		e.setCode( Exception::OCSPCertMissing );
		throw e;
		break;
	}
	default:
	{
		std::ostringstream s;
		s << msg << "\n (error: " << err;
		if( f_getErrorString )
			s << "; message: " << f_getErrorString( err );
		s << ")";
		throwError( s.str(), line );
		break;
	}
	}
}

void DDocPrivate::throwSignError( const char *id, int err, const std::string &msg, int line ) const throw(BDocException)
{
	if( err != ERR_OK )
		f_SignatureInfo_delete( doc, id );
	throwError( err, msg, line );
}



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

std::string SignatureDDOC::getMediaType() const
{
	std::ostringstream s;
	s << m_doc->doc->szFormat << "/" << m_doc->doc->szFormatVer;
	return s.str();
}

X509Cert SignatureDDOC::getOCSPCertificate() const
{
	try { return X509Cert( m_doc->f_ddocSigInfo_GetOCSPRespondersCert( m_doc->doc->pSignatures[m_id] ) ); }
	catch( const Exception & ) {}
	return X509Cert();
}

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

std::string SignatureDDOC::getResponderID() const
{
	NotaryInfo *n = m_doc->doc->pSignatures[m_id]->pNotary;
	return n ? std::string( (const char*)n->mbufRespId.pMem, n->mbufRespId.nLen ) : std::string();
}

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

void SignatureDDOC::validateOffline() const throw(SignatureException)
{
	int err = m_doc->f_verifySignatureAndNotary(
		m_doc->doc, m_doc->doc->pSignatures[m_id], m_doc->filename.c_str() );
	throwError( "Failed to validate signature", err, __LINE__ );
}

OCSP::CertStatus SignatureDDOC::validateOnline() const throw(SignatureException)
{
	int err = m_doc->f_verifySignatureAndNotary(
		m_doc->doc, m_doc->doc->pSignatures[m_id], m_doc->filename.c_str() );
	throwError( "Failed to validate signature", err, __LINE__ );
	return OCSP::GOOD;
}
void SignatureDDOC::sign(Signer* signer) throw(SignatureException, SignException) {}

void SignatureDDOC::throwError( std::string msg, int err, int line ) const throw(SignatureException)
{
	switch( err )
	{
	case ERR_OK: break;
	case ERR_PKCS_LOGIN:
	{
		SignatureException e( __FILE__, line, "PIN Incorrect" );
		e.setCode( Exception::PINIncorrect );
		throw e;
		break;
	}
	case ERR_OCSP_CERT_REVOKED:
	{
		SignatureException e( __FILE__, line, "Certificate status: revoked" );
		e.setCode( Exception::CertificateRevoked );
		throw e;
		break;
	}
	case ERR_OCSP_CERT_UNKNOWN:
	{
		SignatureException e( __FILE__, line, "Certificate status: unknown" );
		e.setCode( Exception::CertificateUnknown );
		throw e;
		break;
	}
	case ERR_OCSP_RESP_NOT_TRUSTED:
	{
		SignatureException e( __FILE__, line, "Failed to find ocsp responder." );
		e.setCode( Exception::OCSPResponderMissing );
		throw e;
		break;
	}
	case ERR_OCSP_CERT_NOTFOUND:
	{
		SignatureException e( __FILE__, line, "OCSP certificate loading failed" );
		e.setCode( Exception::OCSPCertMissing );
		throw e;
		break;
	}
	case ERR_UNKNOWN_CA:
	{
		std::ostringstream s;
		s << msg;
		s << "; error: " << err;
		if( m_doc->f_getErrorString )
			s << "; message: " << m_doc->f_getErrorString( err );
		SignatureException e( __FILE__, line, s.str() );
		e.setCode( Exception::CertificateUnknown );
		throw e;
		break;
	}
	default:
		std::ostringstream s;
		s << msg;
		s << "; error: " << err;
		if( m_doc->f_getErrorString )
			s << "; message: " << m_doc->f_getErrorString( err );
		throw SignatureException( __FILE__, line, s.str() );
	}
}



DDoc::DDoc()
:	d( new DDocPrivate )
{
	if( !d->ready )
		return;
	/*int err =*/ d->f_SignedDoc_new( &d->doc, "DIGIDOC-XML", "1.3" );
	//throwError( err, "Failed to create new document", __LINE__ );
}
DDoc::~DDoc() { delete d; }

/**
 * Opens DDOC container from a file
 */

digidoc::DDoc::DDoc(std::string path) throw(IOException, BDocException)
 :	d( new DDocPrivate )
{
	if( !d->ready )
		d->throwError( "DDoc library not loaded", __LINE__ );

	d->filename = path;
	loadFile();
}

DDoc::DDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
:	d( new DDocPrivate )
{
	if( !d->ready )
		d->throwError( "DDoc library not loaded", __LINE__ );

	d->filename = serializer->getPath();
	loadFile();
}

void DDoc::loadFile()
{
	try
	{
		util::File::createDirectory( d->tmpFolder = util::File::tempDirectory() );
	}
	catch( const Exception & )
	{ d->throwError( "Failed to create temporary directory", __LINE__ ); }

	int err = d->f_ddocSaxReadSignedDocFromFile( &d->doc, d->filename.c_str(), 0, 0 );
	switch( err )
	{
	case ERR_OK:
	case ERR_OCSP_CERT_REVOKED:
	case ERR_OCSP_CERT_UNKNOWN: break;
	default:
		d->f_SignedDoc_free( d->doc );
		d->doc = NULL;
		d->throwError( err, "Failed to open ddoc file", __LINE__ );
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
			d->throwError( err, "Failed to exctract files", __LINE__ );
		}
		d->documents.push_back( doc );
	}

	d->loadSignatures();
}

void DDoc::addDocument( const Document &document ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	DataFile *data = 0;
	int err = d->f_DataFile_new( &data, d->doc, NULL, document.getPath().c_str(),
		CONTENT_EMBEDDED_BASE64, document.getMediaType().c_str(), 0, NULL, 0,
		NULL, CHARSET_UTF_8 );
	d->throwError( err, "Failed to add file '" + document.getPath() + "'" , __LINE__ );

	err = d->f_calculateDataFileSizeAndDigest(
		d->doc, data->szId, document.getPath().c_str(), DIGEST_SHA1 );
	d->throwError( err, "Failed calculate file digest and size", __LINE__ );
	DocumentDDoc doc = { document.getPath(), document.getMediaType() };
	d->documents.push_back( doc );
}

unsigned int DDoc::documentCount() const { return d->documents.size(); }

Document DDoc::getDocument( unsigned int id ) const throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->documents.size() )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->documents.size() << " documents in container.";
		d->throwError( s.str(), __LINE__ );
	}

	return Document( d->documents[id].filename, d->documents[id].mime );
}

const Signature* DDoc::getSignature( unsigned int id ) const throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->signatures.size() )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->signatures.size() << " signatures in container.";
		d->throwError( s.str(), __LINE__ );
	}

	return d->signatures[id];
}

void DDoc::removeDocument( unsigned int id ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->documents.size() )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->documents.size() << " documents in container.";
		d->throwError( s.str(), __LINE__ );
	}
	if( !d->signatures.empty() )
	{
		d->throwError(
			"Can not remove document from container which has signatures, "
			"remove all signatures before removing document.", __LINE__ );
	}

	int err = d->f_DataFile_delete( d->doc, d->doc->pDataFiles[id]->szId );
	d->throwError( err, "Failed to delete file", __LINE__ );
	d->documents.erase( d->documents.begin() + id );
}

void DDoc::removeSignature( unsigned int id ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= d->signatures.size() )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->signatures.size() << " signatures in container.";
		d->throwError( s.str(), __LINE__ );
	}

	int err = d->f_SignatureInfo_delete( d->doc, d->doc->pSignatures[id]->szId );
	d->throwError( err, "Failed to remove signature", __LINE__ );
	d->loadSignatures();
}

void DDoc::save() throw(IOException, BDocException)
{
	d->throwDocOpenError( __LINE__ );
	int err = d->f_createSignedDoc( d->doc, d->filename.c_str(), d->filename.c_str() );
	d->throwError( err, "Failed to save document", __LINE__ );
}

void DDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	d->throwDocOpenError( __LINE__ );
	d->filename = serializer->getPath();
	save();
}

void DDoc::sign( Signer *signer, Signature::Type type ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if ( type == Signature::MOBILE )
	{
		std::string file = signer->signaturePath();
		SignedDoc *sigDoc = 0;
		int err = d->f_ddocSaxReadSignedDocFromFile( &sigDoc, file.c_str(), 0, 0 );
		d->throwError( err, "Failed to sign document", __LINE__ );

		SignatureInfo **signatures = (SignatureInfo**)realloc( d->doc->pSignatures,
			(d->doc->nSignatures + sigDoc->nSignatures) * sizeof(void *));
		if( !signatures )
		{
			d->f_SignedDoc_free( sigDoc );
			d->throwError( "Failed to sign document", __LINE__ );
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

unsigned int DDoc::signatureCount() const { return d->signatures.size(); }

void DDoc::getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException)
{
	d->throwDocOpenError( __LINE__ );

	if( id >= (unsigned int)d->doc->nDataFiles || !d->doc->pDataFiles[id] )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->doc->nDataFiles << " documents in container.";
		d->throwError( s.str(), __LINE__ );
	}

	memcpy( digest, d->doc->pDataFiles[id]->mbufDigest.pMem, d->doc->pDataFiles[id]->mbufDigest.nLen );
}
