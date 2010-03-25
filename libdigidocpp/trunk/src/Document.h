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

#if !defined(__DOCUMENT_H_INCLUDED__)
#define __DOCUMENT_H_INCLUDED__

#include "io/IOException.h"

namespace digidoc
{
    class Digest;
    /**
     * Document wrapper for signed file in BDOC container.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC Document
    {

      public:
          Document(const std::string& path, const std::string& mediaType);
          std::string getPath() const;
          std::string getMediaType() const;
          unsigned long getSize() const throw(IOException);
          std::vector<unsigned char> calcDigest(Digest* calc) throw(IOException);
          void saveAs(const std::string& path) throw(IOException);

      protected:
          std::string path;
          std::string mediaType;
          std::vector<unsigned char> digest;

    };
}

#endif // !defined(__DOCUMENT_H_INCLUDED__)
