#include "WDoc.h"

#include "BDoc.h"
#include "DDoc.h"
#include "Document.h"
#include "io/ISerialize.h"

using namespace digidoc;

/**
 * Initialize container.
 * Default type DDoc
 */
WDoc::WDoc(): m_doc(NULL) { setType( DDocType ); }

/**
 * Initialize container.
 * @param type specify document type
 */
WDoc::WDoc( DocumentType type ): m_doc(NULL) { setType( type ); }

/**
 * Initialize container
 * @param custom document format
 */
WDoc::WDoc( ADoc *doc ) { m_doc = doc; }

/**
 * Releases resources.
 */
WDoc::~WDoc()
{
	if( m_doc )
		delete m_doc;
}

/**
 * Opens container from a file
 */
WDoc::WDoc(std::string path) throw(IOException, BDocException)
{
	std::string ext = path.substr( path.size() - 4 );
	transform( ext.begin(), ext.end(), ext.begin(), tolower );

	if( ext == "bdoc" )
	{
		m_doc = new BDoc( path );
		m_type = BDocType;
	}
	else if( ext == "ddoc" )
	{
		m_doc = new DDoc( path );
		m_type = DDocType;
	}
	else
	{
		m_type = BDocType;
		throw IOException( __FILE__, __LINE__, "Unknown document format" );
	}
}

/**
 * Opens container using <code>serializer</code> implementation.
 *
 * @param serializer implementation of the serializer.
 * @throws IOException exception is thrown if reading data from the container failed.
 * @throws BDocException exception is thrown if the container is not valid.
 */
WDoc::WDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	int len = serializer->getPath().size();
	std::string ext = serializer->getPath().substr( len - 4 );
	transform( ext.begin(), ext.end(), ext.begin(), tolower );

	if( ext == "bdoc" )
	{
		m_doc = new BDoc( serializer );
		m_type = BDocType;
	}
	else if( ext == "ddoc" )
	{
		m_doc = new DDoc( serializer );
		m_type = DDocType;
	}
	else
	{
		m_type = BDocType;
		throw IOException( __FILE__, __LINE__, "Unknown document format" );
	}
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
void WDoc::addDocument(const Document& document) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->addDocument( document );
}

/**
 * @return returns number of documents in container.
 */
unsigned int WDoc::documentCount() const
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->documentCount();
}

/**
 * Returns document referenced by document id.
 *
 * @param id document id.
 * @return returns document referenced by document id.
 * @throws BDocException throws exception if the document id is incorrect.
 */
Document WDoc::getDocument( unsigned int id ) const throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->getDocument( id );
}

/**
 * Returns signature referenced by signature id.
 *
 * @param id signature id.
 * @return returns signature referenced by signature id.
 * @throws BDocException throws exception if the signature id is incorrect.
 */
const Signature* WDoc::getSignature( unsigned int id ) const throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->getSignature( id );
}

/**
 * Removes document from container by document id. Documents can be
 * removed from container only after all signatures are removed.
 *
 * @param id document's id, which will be removed.
 * @throws BDocException throws exception if the document id is incorrect or there are
 *         one or more signatures.
 */
void WDoc::removeDocument( unsigned int id ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->removeDocument( id );
}

/**
 * Removes signature from container by signature id.
 *
 * @param id signature's id, which will be removed.
 * @throws BDocException throws exception if the signature id is incorrect.
 */
void WDoc::removeSignature( unsigned int id ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->removeSignature( id );
}

/**
 * Saves the container using the <code>serializer</code> implementation provided in
 * <code>readFrom()</code> method.
 *
 * @throws IOException is thrown if there was a failure saving BDOC container. For example added
 *         document does not exist.
 * @throws BDocException is thrown if BDoc class is not correctly initialized.
 */
void WDoc::save() throw(IOException, BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->save();
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
void WDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->saveTo( serializer );
}

/**
 * Sets doucment type
 * @param type document type
 */
void WDoc::setType( DocumentType type )
{
	delete m_doc;
	switch( type )
	{
	case BDocType: m_doc = new BDoc(); break;
	case DDocType: m_doc = new DDoc(); break;
	default: m_doc = 0;
	}
	m_type = type;
}

/**
 * Signs all documents in container.
 *
 * @param signer signer implementation.
 * @param profile signature profile (e.g. BES, TM).
 * @throws BDocException exception is throws if signing the BDCO container failed.
 */
void WDoc::sign( Signer *signer, Signature::Type type ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->sign( signer, type );
}

/**
 * @return returns number of signatures in container.
 */
unsigned int WDoc::signatureCount() const
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->signatureCount();
}

/**
 * @return returns current document format
 */
WDoc::DocumentType WDoc::documentType() const
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_type;
}

/**
 * Returns file digest format
 */
void WDoc::getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->getFileDigest( id, digest );
}
