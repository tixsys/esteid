#include "Document.h"
#include "Util.h"

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
    return Util::File::fileSize(path);
}

/**
 * Saves a copy of the document to specified path.
 *
 * @param path full path, where the document should be saved to.
 */
void digidoc::Document::saveTo(const std::string& path) throw(IOException)
{
	Util::File::copyFile(this->path, path);
}
