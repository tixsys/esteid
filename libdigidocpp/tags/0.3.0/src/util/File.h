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

#if !defined(__FILE_H_INCLUDED__)
#define __FILE_H_INCLUDED__

#include "../io/IOException.h"

#include <stack>

namespace digidoc
{
    namespace util
    {

        /**
         * Implements common file-system operations for current platform
         *
         * @author Janari Põld
         */
        class File
        {
          public:
              static bool fileExists(const std::string& path);
              EXP_DIGIDOC static bool directoryExists(const std::string& path);
              static std::string encodeName(const std::string& fileName);
              static std::string decodeName(const std::string& localFileName);
#ifdef _WIN32
              static std::wstring fstreamName(const std::string& fileName);
#else
              static std::string fstreamName(const std::string& fileName);
#endif
              static std::string fileName(const std::string& path);
              static std::string directory(const std::string& path);
              static std::string path(const std::string& directory, const std::string& relativePath, bool unixStyle = false);
              static std::string fullPathUrl(const std::string& fullDirectory, const std::string& relativeFilePath);
              static std::string tempFileName();
              static std::string tempDirectory();
              static void createDirectory(const std::string& path) throw(IOException);
              static std::string createTempDirectory() throw(IOException);
              static unsigned long fileSize(const std::string& path) throw(IOException);
              static std::vector<std::string> listFiles(const std::string& directory, bool relative = false,
            		  bool listEmptyDirectories = false, bool unixStyle = false) throw(IOException);
              static void copyFile(const std::string& srcPath, const std::string& destPath, bool overwrite = true) throw(IOException);
              static void moveFile(const std::string& srcPath, const std::string& destPath, bool overwrite = true) throw(IOException);
              static void removeFile(const std::string& fname);
              static void removeDirectory(const std::string& fname);
              EXP_DIGIDOC static void deleteTempFiles() throw(IOException);
              static void removeDirectoryRecursively(const std::string& dname) throw(IOException);
              static std::stack<std::string> tempFiles;

        private:
              static std::vector<std::string> getDirSubElements(const std::string& directory, bool relative,
            		  bool filesOnly, bool unixStyle) throw(IOException);
        };

    }
}

#endif // !defined(__FILE_H_INCLUDED__)
