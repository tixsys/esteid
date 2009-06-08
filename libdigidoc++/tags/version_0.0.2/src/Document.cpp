#include <fstream>

#include "log.h"
#include "Document.h"
#include "crypto/SHA1Digest.h"
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
 * Calculates SHA1 digest for document. If SHA1 digest is already calculated returns it,
 * otherwise calculates the digest.
 *
 * return returns calculated SHA1 digest.
 * throws IOException throws exception if the file does not exist or SHA1 calculation fails.
 */
std::vector<unsigned char> digidoc::Document::calcSHA1Digest() throw(IOException)
{
	// If SHA1 digest is already calculated return it.
	if(!sha1Digest.empty())
	{
		return sha1Digest;
	}

	// Calculate SHA1 digest.
    std::ifstream ifs(path.c_str(), std::ios::binary);
    if(ifs.fail())
    {
    	THROW_IOEXCEPTION("Failed to open document file '%s'.", path.c_str())
    }

	unsigned int const BUF_SIZE = 10240;
    char buf[BUF_SIZE];
    int bytesRead = 0;
    digidoc::SHA1Digest sha1;
    while((bytesRead = ifs.readsome(buf, BUF_SIZE)) > 0)
    {
        sha1.update(reinterpret_cast<unsigned char*>(buf), bytesRead);
    }
    ifs.close();

    if(ifs.fail())
    {
    	THROW_IOEXCEPTION("Failed to calculate SHA1 digest for document file '%s'.", path.c_str())
    }

    sha1Digest = sha1.getDigest();
    return sha1Digest;
}

/**
 * Saves a copy of the document to specified path.
 *
 * @param path full path, where the document should be saved to.
 */
void digidoc::Document::saveTo(const std::string& path) throw(IOException)
{
    util::File::copyFile(this->path, path);
}
