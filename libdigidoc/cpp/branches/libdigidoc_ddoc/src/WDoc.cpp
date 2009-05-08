#include "WDoc.h"

#include "BDoc.h"
#include "DDoc.h"

using namespace digidoc;

WDoc::WDoc()
{
	m_doc = new BDoc();
	m_type = BDocType;
}

WDoc::~WDoc() { delete m_doc; }

WDoc::WDoc( ADoc *doc )
{
	BDoc *bdoc = static_cast<BDoc*>( doc );
	if( bdoc )
		m_type = BDocType;
	DDoc *ddoc = static_cast<DDoc*>( doc );
	if( ddoc )
		m_type = DDocType;

	if( !bdoc && !ddoc )
		m_type = CustomType;

	m_doc = doc;
}

WDoc::WDoc(std::auto_ptr<ISerialize> serializer) throw(IOException, BDocException)
{
	int len = serializer->getPath().size();
	std::string ext = serializer->getPath().substr( len - 4, len );

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
		m_type = CustomType;
		throw IOException( __FILE__, __LINE__, "Unknow document format" );
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

WDoc::DocumentType WDoc::type() const { return m_type; }
