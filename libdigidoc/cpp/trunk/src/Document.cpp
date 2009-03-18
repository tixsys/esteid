#include <fstream>

#include <openssl/objects.h>

#include "log.h"
#include "Document.h"
#include "util/File.h"
#include "util/String.h"

/**
 * Initializes the document object.
 *
 * @param path full path of the document.
 * @param mediaType document media type (e.g. "application/msword" or "text/xml").
 */
digidoc::Document::Document(const std::string& path, const std::string& mediaType)
 : path(path)
 , mediaType(mediaType)
{
}

/**
 * @return returns document full path.
 */
std::string digidoc::Document::getPath() const
{
    return path;
}

/**
 * @return returns documents media type.
 * @ see Document::Document(const std::string& fullPath, const std::string& mediaType)
 */
std::string digidoc::Document::getMediaType() const
{
    return mediaType;
}

/**
 * @returns returns the document file size.
 */
unsigned long digidoc::Document::getSize() const throw(IOException)
{
    return util::File::fileSize(path);
}

/**
 * Calculates digest for document. If digest is already calculated returns it,
 * otherwise calculates the digest.
 *
 * @param calc digest calculator implementation.
 * return returns calculated digest.
 * throws IOException throws exception if the file does not exist or digest calculation fails.
 */
std::vector<unsigned char> digidoc::Document::calcDigest(Digest* calc) throw(IOException)
{
	// If digest is already calculated return it.
	if(!digest.empty())
	{
	    DEBUG("Digest already calculated");
		return digest;
	}

	// Calculate digest.
    std::ifstream ifs; 
	ifs.open(path.c_str(), std::ios::in | std::ios::binary);
	
	if(!ifs.is_open() || ifs.fail())
    {
    	THROW_IOEXCEPTION("Failed to open document file '%s'.", path.c_str());
    }

	unsigned int const BUF_SIZE = 10240;
    while( ifs )
    {
	    char buf[BUF_SIZE];
		ifs.read( buf, BUF_SIZE );
		int bytesRead = ifs.gcount();
		if ( bytesRead > 0 )
		{
			DEBUG("Added %d bytes from %s", bytesRead, path.c_str());
			calc->update(reinterpret_cast<unsigned char*>(buf), bytesRead);
		}
    }

    digest = calc->getDigest();
    DEBUGMEM("digest", &digest[0], digest.size());
    return digest;
}

/**
 * Saves a copy of the document to specified path.
 * @param path full file path, where the document should be saved to. If file exists, it is overwritten
 * throws IOException if part of path does not exist or path is existing directory (without file name)
 */
void digidoc::Document::saveTo(const std::string& path) throw(IOException)
{
    util::File::copyFile(this->path, path);
}
