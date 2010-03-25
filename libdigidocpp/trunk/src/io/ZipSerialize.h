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

#if !defined(__ZIPSERIALIZE_H_INCLUDED__)
#define __ZIPSERIALIZE_H_INCLUDED__

#include "ISerialize.h"

namespace digidoc
{
    /**
     * ZIP file implementation of the ISerialize interface. Saves files to ZIP file
     * and extracts the ZIP file on demand. Uses ZLib to implement ZIP file operations.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC ZipSerialize : public ISerialize
    {

      public:
          ZipSerialize(const std::string& path);
          std::string extract() throw(IOException);
          void create();
          void addFile(const std::string& containerPath, const std::string& path);
          void save() throw(IOException);

      protected:
          struct FileEntry { std::string containerPath; std::string path; };
          std::vector<FileEntry> filesAdded;

    };
}

#endif // !defined(__ZIPSERIALIZE_H_INCLUDED__)
