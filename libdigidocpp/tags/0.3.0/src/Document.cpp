/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "log.h"
#include "Document.h"
#include "crypto/Digest.h"
#include "util/File.h"
#include "util/String.h"

#include <fstream>

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
 * otherwise calculates the digest using a default Digest implementation.
 *
 * return returns calculated digest.
 * throws IOException throws exception if the file does not exist or digest calculation fails.
 */
std::vector<unsigned char> digidoc::Document::calcDigest() throw(IOException)
{
    // If digest is already calculated return it.
    if(!digest.empty())
    {
        DEBUG("Digest already calculated");
        return digest;
    }

    std::auto_ptr<Digest> calc = Digest::create();
    return calcDigest(calc.get());
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
    if(calc == NULL)
    {
        THROW_IOEXCEPTION("Null pointer in Documnent::calcDigest");
    }

    // If digest is already calculated return it.
    if(!digest.empty())
    {
        DEBUG("Digest already calculated");
        return digest;
    }

    // Calculate digest.
    std::ifstream ifs(digidoc::util::File::fstreamName(path).c_str(), std::ios::in | std::ios::binary);
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
 * Saves a copy of the document as file specified by path.
 * @param path full file path, where the document should be saved to. If file exists, it is overwritten
 * throws IOException if part of path does not exist or path is existing directory (without file name)
 */
void digidoc::Document::saveAs(const std::string& path) throw(IOException)
{
    util::File::copyFile(this->path, path);
}
