#include "WDoc.h"

#include "BDoc.h"
#include "DDoc.h"
#include "Document.h"
#include "io/ISerialize.h"

using namespace digidoc;

WDoc::WDoc(): m_doc(NULL) { setType( DDocType ); }
WDoc::WDoc( DocumentType type ): m_doc(NULL) { setType( type ); }
WDoc::WDoc( ADoc *doc ) { m_doc = doc; }

WDoc::~WDoc()
{
	if( m_doc )
		delete m_doc;
}

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

void WDoc::addDocument(const Document& document) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->addDocument( document );
}

unsigned int WDoc::documentCount() const
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->documentCount();
}

Document WDoc::getDocument( unsigned int id ) const throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->getDocument( id );
}

const Signature* WDoc::getSignature( unsigned int id ) const throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->getSignature( id );
}

void WDoc::removeDocument( unsigned int id ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->removeDocument( id );
}

void WDoc::removeSignature( unsigned int id ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->removeSignature( id );
}

void WDoc::save() throw(IOException, BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->save();
}

void WDoc::saveTo(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->saveTo( serializer );
}

void WDoc::setType( DocumentType type )
{
	if( m_doc ) delete m_doc;
	switch( type )
	{
	case BDocType: m_doc = new BDoc(); break;
	case DDocType: m_doc = new DDoc(); break;
	default: m_doc = 0;
	}
	m_type = type;
}

void WDoc::sign( Signer *signer, Signature::Type type ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->sign( signer, type );
}

unsigned int WDoc::signatureCount() const
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_doc->signatureCount();
}

WDoc::DocumentType WDoc::documentType() const
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	return m_type;
}

void WDoc::getFileDigest( unsigned int id, unsigned char *digest ) throw(BDocException)
{
	if( !m_doc )
		throw BDocException( __FILE__, __LINE__, "Document not open" );

	m_doc->getFileDigest( id, digest );
}