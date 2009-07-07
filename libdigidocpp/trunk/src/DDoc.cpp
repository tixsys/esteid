#include "DDoc_p.h"
#include "DDoc.h"

#include "crypto/signer/PKCS11Signer.h"
#include "util/File.h"
#include "Conf.h"
#include "XmlConf.h"

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
,	f_addAllDocInfos(0)
,	f_addSignerRole(0)
,	f_calculateDataFileSizeAndDigest(0)
,	f_calculateSignatureWithEstID(0)
,	f_cleanupConfigStore(0)
,	f_ConfigItem_lookup_int(0)
,	f_convertStringToTimestamp(0)
,	f_createOrReplacePrivateConfigItem(0)
,	f_createSignedDoc(0)
,	f_DataFile_delete(0)
,	f_DataFile_new(0)
,	f_ddocReadNewSignaturesFromDdoc(0)
,	f_ddocSaxReadSignedDocFromFile(0)
,	f_ddocSigInfo_GetSignersCert(0)
,	f_finalizeDigiDocLib(0)
,	f_getErrorString(0)
,	f_initDigiDocLib(0)
,	f_initConfigStore(0)
,	f_notarizeSignature(0)
,	f_ddocSaxExtractDataFile(0)
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
	for( std::vector<DSignature*>::iterator i = signatures.begin();
		i != signatures.end(); ++i )
		delete *i;
	if( !f_SignedDoc_free || !f_cleanupConfigStore || !f_finalizeDigiDocLib )
		return;
	if( doc )
		f_SignedDoc_free( doc );
	f_cleanupConfigStore( NULL );
	f_finalizeDigiDocLib();
}

bool DDocPrivate::isLoaded() const { return ready; }

void DDocPrivate::loadSignatures()
{
	for( std::vector<DSignature*>::iterator i = signatures.begin();
		i != signatures.end(); ++i )
		delete *i;
	signatures.clear();
	for( int i = 0; i < doc->nSignatures; ++i )
		signatures.push_back( new DSignature( i, this ) );
}

bool DDocPrivate::loadSymbols()
{
	if( !(f_addAllDocInfos = (sym_addAllDocInfos)lib.resolve("addAllDocInfos")) ||
		!(f_addSignerRole = (sym_addSignerRole)lib.resolve("addSignerRole")) ||
		!(f_calculateDataFileSizeAndDigest = (sym_calculateDataFileSizeAndDigest)lib.resolve("calculateDataFileSizeAndDigest")) ||
		!(f_calculateSignatureWithEstID = (sym_calculateSignatureWithEstID)lib.resolve("calculateSignatureWithEstID")) ||
		!(f_cleanupConfigStore = (sym_cleanupConfigStore)lib.resolve("cleanupConfigStore")) ||
		!(f_ConfigItem_lookup_int = (sym_ConfigItem_lookup_int)lib.resolve("ConfigItem_lookup_int")) ||
		!(f_convertStringToTimestamp = (sym_convertStringToTimestamp)lib.resolve("convertStringToTimestamp")) ||
		!(f_createOrReplacePrivateConfigItem = (sym_createOrReplacePrivateConfigItem)lib.resolve("createOrReplacePrivateConfigItem")) ||
		!(f_createSignedDoc = (sym_createSignedDoc)lib.resolve("createSignedDoc")) ||
		!(f_DataFile_delete = (sym_DataFile_delete)lib.resolve("DataFile_delete")) ||
		!(f_DataFile_new = (sym_DataFile_new)lib.resolve("DataFile_new")) ||
		!(f_ddocReadNewSignaturesFromDdoc = (sym_ddocReadNewSignaturesFromDdoc)lib.resolve("ddocReadNewSignaturesFromDdoc")) ||
		!(f_ddocSaxReadSignedDocFromFile = (sym_ddocSaxReadSignedDocFromFile)lib.resolve("ddocSaxReadSignedDocFromFile")) ||
		!(f_ddocSigInfo_GetSignersCert = (sym_ddocSigInfo_GetSignersCert)lib.resolve("ddocSigInfo_GetSignersCert")) ||
		!(f_finalizeDigiDocLib = (sym_finalizeDigiDocLib)lib.resolve("finalizeDigiDocLib")) ||
		!(f_getErrorString = (sym_getErrorString)lib.resolve("getErrorString")) ||
		!(f_initDigiDocLib = (sym_initDigiDocLib)lib.resolve("initDigiDocLib")) ||
		!(f_initConfigStore = (sym_initConfigStore)lib.resolve("initConfigStore")) ||
		!(f_notarizeSignature = (sym_notarizeSignature)lib.resolve("notarizeSignature")) ||
		!(f_ddocSaxExtractDataFile = (sym_ddocSaxExtractDataFile)lib.resolve("ddocSaxExtractDataFile")) ||
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


DSignature::DSignature( int id, DDocPrivate *doc )
:	m_id( id ), m_doc( doc )
{
	SignatureInfo *sig = doc->doc->pSignatures[id];
	X509 *cert = doc->f_ddocSigInfo_GetSignersCert( sig );
	try { setSigningCertificate( cert ); }
	catch( const Exception &e ) {}

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

void DSignature::getRevocationOCSPRef(std::vector<unsigned char>& data, std::string& digestMethodUri) const throw(SignatureException)
{
	NotaryInfo *n = m_doc->doc->pSignatures[m_id]->pNotary;
	data.resize( n->mbufOcspDigest.nLen );
	std::copy( (unsigned char*)n->mbufOcspDigest.pMem,
		(unsigned char*)n->mbufOcspDigest.pMem + n->mbufOcspDigest.nLen, data.begin() );
	digestMethodUri = n->szDigestType;
}

void DSignature::validateOffline() const throw(SignatureException)
{
	int err = m_doc->f_verifySignatureAndNotary(
		m_doc->doc, m_doc->doc->pSignatures[m_id], m_doc->filename.c_str() );
	throwError( "Failed to validate signature", err, __LINE__ );
}

OCSP::CertStatus DSignature::validateOnline() const throw(SignatureException)
{
	int err = m_doc->f_verifySignatureAndNotary(
		m_doc->doc, m_doc->doc->pSignatures[m_id], m_doc->filename.c_str() );
	throwError( "Failed to validate signature", err, __LINE__ );
	return OCSP::GOOD;
}
void DSignature::sign(Signer* signer) throw(SignatureException, SignException) {}

void DSignature::throwError( std::string msg, int err, int line ) const throw(SignatureException)
{
	if( err == ERR_OK )
		return;

	std::ostringstream s;
	s << msg;
	s << "; error: " << err;
	if( m_doc->f_getErrorString )
		s << "; message: " << m_doc->f_getErrorString( err );
	throw SignatureException( __FILE__, line, s.str() );
}



DDoc::DDoc()
{
	d = new DDocPrivate();
	if( !d->isLoaded() )
		return;
	/*int err =*/ d->f_SignedDoc_new( &d->doc, "DIGIDOC-XML", "1.3" );
	//throwError( err, "Failed to create new document", __LINE__ );
}
DDoc::~DDoc() { delete d; }

DDoc::DDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	d = new DDocPrivate();
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );

	d->filename = serializer->getPath();
	try
	{
		util::File::createDirectory( d->tmpFolder = util::File::tempDirectory() );
	}
	catch( const Exception & )
	{ throwError( "Failed to create temporary directory", __LINE__ ); }

	int err = d->f_ddocSaxReadSignedDocFromFile( &d->doc, d->filename.c_str(), 0, 300 );
	throwError( err, "Failed to open ddoc file", __LINE__ );

	for( int i = 0; i < d->doc->nDataFiles; ++i)
	{
		DataFile *data = d->doc->pDataFiles[i];
		std::ostringstream file;
		file << d->tmpFolder.data() << data->szFileName;
		free( data->szFileName );
		data->szFileName = strdup( file.str().data() );
		if ( !strcmp( data->szContentType, CONTENT_HASHCODE ) )
			continue;
		int err = d->f_ddocSaxExtractDataFile( d->doc, d->filename.c_str(),
			file.str().data(), data->szId, CHARSET_UTF_8 );
		throwError( err, "Failed to exctract files", __LINE__ );
	}

	d->loadSignatures();
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
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );
	if( id >= (unsigned int)d->doc->nDataFiles || !d->doc->pDataFiles[id] )
	{
		std::ostringstream s;
		s << "Incorrect document id " << id << ", there are only ";
		s << d->doc->nDataFiles << " documents in container.";
		throwError( s.str(), __LINE__ );
	}

	return Document( d->doc->pDataFiles[id]->szFileName, d->doc->pDataFiles[id]->szMimeType );
}

const Signature* DDoc::getSignature( unsigned int id ) const throw(BDocException)
{
	if( !d->doc )
		throwError( "Document not open", __LINE__ );
	if( id >= (unsigned int)d->doc->nSignatures || !d->doc->pSignatures[id] )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->doc->nDataFiles << " signatures in container.";
		throwError( s.str(), __LINE__ );
	}

	return d->signatures[id];
}

void DDoc::removeDocument( unsigned int id ) throw(BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	if( id >= (unsigned int)d->doc->nDataFiles || !d->doc->pDataFiles[id] )
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

	if( id >= (unsigned int)d->doc->nSignatures || !d->doc->pSignatures[id] )
	{
		std::ostringstream s;
		s << "Incorrect signature id " << id << ", there are only ";
		s << d->doc->nDataFiles << " signatures in container.";
		throwError( s.str(), __LINE__ );
	}

	int err = d->f_SignatureInfo_delete( d->doc, d->doc->pSignatures[id]->szId );
	throwError( err, "Failed to remove signature", __LINE__ );
	d->loadSignatures();
}

void DDoc::save() throw(IOException, BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	int err = d->f_createSignedDoc( d->doc, d->filename.c_str(), d->filename.c_str() );
	throwError( err, "Failed to save document", __LINE__ );
}

void DDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	d->filename = serializer->getPath();
	save();
}

void DDoc::sign( Signer *signer, Signature::Type type ) throw(BDocException)
{
	if( !d->isLoaded() )
		throwError( "DDoc library not loaded", __LINE__ );
	if( !d->doc )
		throwError( "Document not open", __LINE__ );

	if ( type == Signature::MOBILE )
	{
		std::string file = signer->signaturePath();
		d->f_ddocReadNewSignaturesFromDdoc(d->doc, file.c_str());
		d->loadSignatures();
		return;
	}

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
	for( size_t i = 0; i < r.size(); ++i )
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
		d->f_SignatureInfo_delete( d->doc, info->szId );
	throwError( err, "Failed to sign document", __LINE__ );

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
	if( err != ERR_OK )
		d->f_SignatureInfo_delete( d->doc, info->szId );
	throwError( err, "Failed to sign document", __LINE__ );
	d->loadSignatures();
}

unsigned int DDoc::signatureCount() const
{ return !d->doc || d->doc->nSignatures < 0 ? 0 : d->doc->nSignatures; }

void DDoc::throwError( const std::string &msg, int line ) const throw(BDocException)
{ throw BDocException( __FILE__, line, msg ); }

void DDoc::throwError( int err, const std::string &msg, int line ) const throw(BDocException)
{
	if( err != ERR_OK )
	{
		std::ostringstream s;
		s << msg << "\n (error: " << err;
		if( d->f_getErrorString )
			s << "; message: " << d->f_getErrorString( err );
		s << ")";
		throwError( s.str(), line );
	}
}
