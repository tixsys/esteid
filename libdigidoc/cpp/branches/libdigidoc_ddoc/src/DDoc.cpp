#include "DDoc_p.h"
#include "DDoc.h"

#include "crypto/signer/PKCS11Signer.h"

using namespace digidoc;

#ifdef WIN32

#define LIBDIGIDOC_NAME "digidoc2.dll"

DDocLibrary::DDocLibrary()
{ h = LoadLibrary( LIBDIGIDOC_NAME ); }

DDocLibrary::~DDocLibrary()
{ if( isOpen() ) FreeLibrary( h ); }

void* DDocLibrary::resolve( const char *symbol )
{ return isOpen() ? (void*)GetProcAddress( h, symbol ) : NULL; }

#else

#if defined(__APPLE__)
#define LIBDIGIDOC_NAME "libdigidoc2.dylib"
#else
#define LIBDIGIDOC_NAME "libdigidoc2.so"
#endif

DDocLibrary::DDocLibrary()
{ h = dlopen( LIBDIGIDOC_NAME, RTLD_LAZY ); }

DDocLibrary::~DDocLibrary()
{ if( isOpen() ) dlclose( h ); }

void* DDocLibrary::resolve( const char *symbol )
{ return isOpen() ? dlsym( h, symbol ) : NULL; }

#endif



DDocPrivate::DDocPrivate()
:	doc(0)
,	filename(0)
,	ready(false)
,	f_addAllDocInfos(0)
,	f_addSignerRole(0)
,	f_calculateDataFileSizeAndDigest(0)
,	f_calculateSignatureWithEstID(0)
,	f_cleanupConfigStore(0)
,	f_ConfigItem_lookup_int(0)
,	f_convertStringToTimestamp(0)
,	f_createSignedDoc(0)
,	f_DataFile_delete(0)
,	f_DataFile_new(0)
,	f_ddocSaxReadSignedDocFromFile(0)
,	f_ddocSigInfo_GetSignersCert(0)
,	f_finalizeDigiDocLib(0)
,	f_getErrorString(0)
,	f_initDigiDocLib(0)
,	f_initConfigStore(0)
,	f_notarizeSignature(0)
,	f_setSignatureProductionPlace(0)
,	f_SignatureInfo_delete(0)
,	f_SignatureInfo_new(0)
,	f_SignedDoc_free(0)
,	f_SignedDoc_new(0)
,	f_verifySignatureAndNotary(0)
{
	if( !loadSymbols() )
		return;

	f_initDigiDocLib();
	f_initConfigStore( NULL );
	ready = true;
}

DDocPrivate::~DDocPrivate()
{
	if( !f_SignedDoc_free || !f_cleanupConfigStore || !f_finalizeDigiDocLib )
		return;
	if( doc )
		f_SignedDoc_free( doc );
	f_cleanupConfigStore( NULL );
	f_finalizeDigiDocLib();
}

bool DDocPrivate::isLoaded() const { return ready; }
bool DDocPrivate::loadSymbols()
{
	if( !(f_addAllDocInfos = (sym_addAllDocInfos)lib.resolve("addAllDocInfos")) ||
		!(f_addSignerRole = (sym_addSignerRole)lib.resolve("addSignerRole")) ||
		!(f_calculateDataFileSizeAndDigest = (sym_calculateDataFileSizeAndDigest)lib.resolve("calculateDataFileSizeAndDigest")) ||
		!(f_calculateSignatureWithEstID = (sym_calculateSignatureWithEstID)lib.resolve("calculateSignatureWithEstID")) ||
		!(f_cleanupConfigStore = (sym_cleanupConfigStore)lib.resolve("cleanupConfigStore")) ||
		!(f_ConfigItem_lookup_int = (sym_ConfigItem_lookup_int)lib.resolve("ConfigItem_lookup_int")) ||
		!(f_convertStringToTimestamp = (sym_convertStringToTimestamp)lib.resolve("convertStringToTimestamp")) ||
		!(f_createSignedDoc = (sym_createSignedDoc)lib.resolve("createSignedDoc")) ||
		!(f_DataFile_delete = (sym_DataFile_delete)lib.resolve("DataFile_delete")) ||
		!(f_DataFile_new = (sym_DataFile_new)lib.resolve("DataFile_new")) ||
		!(f_ddocSaxReadSignedDocFromFile = (sym_ddocSaxReadSignedDocFromFile)lib.resolve("ddocSaxReadSignedDocFromFile")) ||
		!(f_ddocSigInfo_GetSignersCert = (sym_ddocSigInfo_GetSignersCert)lib.resolve("ddocSigInfo_GetSignersCert")) ||
		!(f_finalizeDigiDocLib = (sym_finalizeDigiDocLib)lib.resolve("finalizeDigiDocLib")) ||
		!(f_getErrorString = (sym_getErrorString)lib.resolve("getErrorString")) ||
		!(f_initDigiDocLib = (sym_initDigiDocLib)lib.resolve("initDigiDocLib")) ||
		!(f_initConfigStore = (sym_initConfigStore)lib.resolve("initConfigStore")) ||
		!(f_notarizeSignature = (sym_notarizeSignature)lib.resolve("notarizeSignature")) ||
		!(f_setSignatureProductionPlace = (sym_setSignatureProductionPlace)lib.resolve("setSignatureProductionPlace")) ||
		!(f_SignatureInfo_delete = (sym_SignatureInfo_delete)lib.resolve("SignatureInfo_delete")) ||
		!(f_SignatureInfo_new = (sym_SignatureInfo_new)lib.resolve("SignatureInfo_new")) ||
		!(f_SignedDoc_free = (sym_SignedDoc_free)lib.resolve("SignedDoc_free")) ||
		!(f_SignedDoc_new = (sym_SignedDoc_new)lib.resolve("SignedDoc_new")) ||
		!(f_verifySignatureAndNotary = (sym_verifySignatureAndNotary)lib.resolve("verifySignatureAndNotary")) )
		return false;
	else
		return true;
}


DSignature::DSignature(): m_doc(0), m_sig(0) {}
DSignature::DSignature( SignatureInfo *sig, DDocPrivate *doc )
:	m_sig(sig), m_doc( doc )
{
	setSigningCertificate( doc->f_ddocSigInfo_GetSignersCert( sig ) );

	Signer::Signature s;
	s.signature = (unsigned char*)sig->pSigValue->mbufSignatureValue.pMem;
	s.length = sig->pSigValue->mbufSignatureValue.nLen;
	setSignatureValue( s );

	Signer::SignatureProductionPlace l;
	if( sig->sigProdPlace.szCity )
		l.city = sig->sigProdPlace.szCity;
	if( sig->sigProdPlace.szStateOrProvince )
		l.stateOrProvince = sig->sigProdPlace.szStateOrProvince;
	if( sig->sigProdPlace.szPostalCode )
		l.postalCode = sig->sigProdPlace.szPostalCode;
	if( sig->sigProdPlace.szCountryName )
		l.countryName = sig->sigProdPlace.szCountryName;
	setSignatureProductionPlace( l );

	Signer::SignerRole r;
	for( int i = 0; i < sig->signerRole.nClaimedRoles; ++i )
		r.claimedRoles.push_back( sig->signerRole.pClaimedRoles[i] );
	setSignerRole( r );

	Timestamp ts;
	doc->f_convertStringToTimestamp( m_doc->doc, sig->szTimeStamp, &ts );
	setSigningTime( xml_schema::DateTime( ts.year, ts.mon, ts.day, ts.hour, ts.min, ts.sec ) );
}

std::string DSignature::getMediaType() const
{
	std::ostringstream s;
	s << m_doc->doc->szFormat << "/" << m_doc->doc->szFormatVer;
	return s.str();
}

void DSignature::validateOffline() const throw(SignatureException)
{
//	int rc = verifySigCert( sig );
}

OCSP::CertStatus DSignature::validateOnline() const throw(SignatureException)
{
	int err = m_doc->f_verifySignatureAndNotary( m_doc->doc, m_sig, m_doc->filename );
	switch( err )
	{
	case ERR_OK: return OCSP::GOOD;
	case ERR_OCSP_CERT_REVOKED: return OCSP::REVOKED;
	default: return OCSP::UNKNOWN;
	}
}
void DSignature::sign(Signer* signer) throw(SignatureException, SignException) {}



DDoc::DDoc()
{
	d = new DDocPrivate();
	if( !d->isLoaded() )
		return;
	int err = d->f_SignedDoc_new( &d->doc, "DIGIDOC-XML", "1.3" );
	//throwError( err, "Failed to create new document", __LINE__ );
}
DDoc::~DDoc() { delete d; }

DDoc::DDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	d = new DDocPrivate();
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );

	d->filename = serializer->getPath().c_str();
	int err = d->f_ddocSaxReadSignedDocFromFile( &d->doc, d->filename, 0, 300 );
	throwError( err, "Failed to open ddoc file", __LINE__ );
}

void DDoc::addDocument( const Document &document ) throw(BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	DataFile *data;
	int err = d->f_DataFile_new( &data, d->doc, NULL, document.getPath().c_str(),
		CONTENT_EMBEDDED_BASE64, document.getMediaType().c_str(), 0, NULL, 0,
		NULL, CHARSET_UTF_8 );
	throwError( err, "Failed to add file '" + document.getPath() + "'" , __LINE__ );

	err = d->f_calculateDataFileSizeAndDigest(
		d->doc, data->szId, document.getPath().c_str(), DIGEST_SHA1 );
	throwError( err, "Failed calculate file digest and size", __LINE__ );
}

unsigned int DDoc::documentCount() const
{ return !d->doc || d->doc->nDataFiles < 0 ? 0 : d->doc->nDataFiles; }

Document DDoc::getDocument( unsigned int id ) const throw(BDocException)
{
	if( !d->doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );
	if( id >= d->doc->nDataFiles || !d->doc->pDataFiles[id] )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->doc->nDataFiles << " documents in container.";
		throw BDocException( __FILE__, __LINE__, s.str() );
	}

	return Document( d->doc->pDataFiles[id]->szFileName, d->doc->pDataFiles[id]->szMimeType );
}

const Signature* DDoc::getSignature( unsigned int id ) const throw(BDocException)
{
	if( !d->doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );
	if( id >= d->doc->nSignatures || !d->doc->pSignatures[id] )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->doc->nDataFiles << " signatures in container.";
		throw BDocException( __FILE__, __LINE__, s.str() );
	}

	return new DSignature( d->doc->pSignatures[id], d );
}

void DDoc::removeDocument( unsigned int id ) throw(BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	if( id >= d->doc->nDataFiles || !d->doc->pDataFiles[id] )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->doc->nDataFiles << " documents in container.";
		throwError( s.str(), __LINE__ );
	}
	if( d->doc->nSignatures )
	{
		throwError(
			"Can not remove document from container which has signatures, "
			"remove all signatures before removing document.", __LINE__ );
	}

	int err = d->f_DataFile_delete( d->doc, d->doc->pDataFiles[id]->szId );
	throwError( err, "Failed to delete file", __LINE__ );
}

void DDoc::removeSignature( unsigned int id ) throw(BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	if( id >= d->doc->nSignatures || !d->doc->pSignatures[id] )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->doc->nDataFiles << " signatures in container.";
		throwError( s.str(), __LINE__ );
	}

	int err = d->f_SignatureInfo_delete( d->doc, d->doc->pSignatures[id]->szId );
	throwError( err, "Failed to remove signature", __LINE__ );
}

void DDoc::save() throw(IOException, BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	int err = d->f_createSignedDoc( d->doc, d->filename, d->filename );
	throwError( err, "Filed to save document", __LINE__ );
}

void DDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	d->filename = serializer->getPath().c_str();
	save();
}

void DDoc::sign( Signer *signer, Signature::Type type ) throw(BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	SignatureInfo *info;
	int err = d->f_SignatureInfo_new( &info, d->doc, NULL );
	throwError( err, "Failed to sign document", __LINE__ );
	err = d->f_addAllDocInfos( d->doc, info );
	throwError( err, "Failed to sign document", __LINE__ );

	Signer::SignatureProductionPlace l = signer->getSignatureProductionPlace();
	err = d->f_setSignatureProductionPlace( info, l.city.c_str(), l.stateOrProvince.c_str(),
		l.postalCode.c_str(), l.countryName.c_str() );
	throwError( err, "Failed to sign document", __LINE__ );

	Signer::SignerRole::TRoles r = signer->getSignerRole().claimedRoles;
	for( int i = 0; i < r.size(); ++i )
	{
		err = d->f_addSignerRole( info, 0, r[i].c_str(), -1, 0 );
		throwError( err, "Failed to sign document", __LINE__ );
	}

	std::string pin;
	if( PKCS11Signer *pkcs11 = static_cast<PKCS11Signer*>(signer) )
	{
		PKCS11Signer::PKCS11Cert c;
		c.cert = pkcs11->getCert();
		pin = pkcs11->getPin( c );
		pkcs11->unloadDriver();
	}
	err = d->f_calculateSignatureWithEstID( d->doc, info,
		d->f_ConfigItem_lookup_int( "DIGIDOC_SIGNATURE_SLOT", 0 ), pin.c_str() );
	throwError( err, "Failed to sign document", __LINE__ );

	if( err != ERR_OK )
	{
		d->f_SignatureInfo_delete( d->doc, info->szId );
		throwError( err, "Failed to sign document", __LINE__ );
	}

	err = d->f_notarizeSignature( d->doc, info );
	throwError( err, "Failed to sign document", __LINE__ );
}

unsigned int DDoc::signatureCount() const
{ return !d->doc || d->doc->nSignatures < 0 ? 0 : d->doc->nSignatures; }

void DDoc::throwError( const std::string &msg, int line ) throw(BDocException)
{ throw BDocException( __FILE__, line, msg ); }

void DDoc::throwError( int err, const std::string &msg, int line ) throw(BDocException)
{
	if( err != ERR_OK )
	{
		std::ostringstream s;
		s << msg << " (error: " << err;
		if( d->f_getErrorString )
			s << "; message: " << d->f_getErrorString( err );
		s << ")";
		throwError( s.str(), line );
	}
}
