#if !defined(__FILE_H_INCLUDED__)
#define __FILE_H_INCLUDED__

#include <string>
#include <stack>

#include "../io/IOException.h"

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
              static bool directoryExists(const std::string& path);
              static std::string encodeName(const std::string& fileName);
              static std::string decodeName(const std::string& localFileName);
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
              static void deleteTempFiles() throw(IOException);
	      static void removeDirectoryRecursively(const std::string& dname) throw(IOException);
	      static std::stack<std::string> tempFiles;

        private:
              static std::vector<std::string> getDirSubElements(const std::string& directory, bool relative,
            		  bool filesOnly, bool unixStyle) throw(IOException);
        };

    }
}

#endif // !defined(__FILE_H_INCLUDED__)
