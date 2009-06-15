#if !defined(__FILE_H_INCLUDED__)
#define __FILE_H_INCLUDED__

#include <string>

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
              static std::string fileName(const std::string& path);
              static std::string directory(const std::string& path);
              static std::string path(const std::string& directory, const std::string& relativePath);
              static std::string tempFileName();
              static std::string tempDirectory();
              static void createDirectory(const std::string& path) throw(IOException);
              static std::string createTempDirectory() throw(IOException);
              static unsigned long fileSize(const std::string& path) throw(IOException);
              static std::vector<std::string> listFiles(const std::string& directory, bool relative = true) throw(IOException);
              static void copyFile(const std::string& srcPath, const std::string& destPath, bool overwrite = true) throw(IOException);
              static void moveFile(const std::string& srcPath, const std::string& destPath, bool overwrite = true) throw(IOException);

        private:
              static std::vector<std::string> getDirSubElements(const std::string& directory, bool relative, bool getFiles) throw(IOException);
              static std::string pathAsUnix(const std::string& directory, const std::string& relativePath);
        };

    }
}

#endif // !defined(__FILE_H_INCLUDED__)
