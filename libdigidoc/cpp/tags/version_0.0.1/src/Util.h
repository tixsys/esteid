#if !defined(__UTIL_H_INCLUDED__)
#define __UTIL_H_INCLUDED__

#include <memory>
#include <string>

#include "io/IOException.h"

namespace digidoc
{
    class Util
    {

      public:

        /**
         * Implements common string operations not provided by standard library.
         *
         * @author Janari Põld
         */
        class String
        {
          public:
              static std::string format(const char *fmt, ...);
              static std::auto_ptr<char> pointer(const std::string& str);

          private:
              static std::string formatArgList(const char *fmt, va_list args);

        };

        /**
         * Implements common file operations.
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
              static void createDirectory(const std::string& path, __mode_t mode = 0700) throw(IOException);
              static std::string createTempDirectory() throw(IOException);
              static unsigned long fileSize(const std::string& path) throw(IOException);
              static std::vector<std::string> listFiles(const std::string& directory, bool relative = true) throw(IOException);
              static void copyFile(const std::string& srcPath, const std::string& destPath, bool overwrite = true) throw(IOException);
              static void moveFile(const std::string& srcPath, const std::string& destPath, bool overwrite = true) throw(IOException);

        };

    };
}

#endif // !defined(__UTIL_H_INCLUDED__)
