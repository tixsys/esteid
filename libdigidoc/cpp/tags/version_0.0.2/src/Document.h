#if !defined(__DOCUMENT_H_INCLUDED__)
#define __DOCUMENT_H_INCLUDED__

#include <string>
#include <vector>

#include "io/IOException.h"

namespace digidoc
{
    /**
     * Document wrapper for signed file in BDOC container.
     *
     * @author Janari Põld
     */
    class Document
    {

      public:
          Document(const std::string& path, const std::string& mediaType);
          std::string getPath() const;
          std::string getMediaType() const;
          unsigned long getSize() const throw(IOException);
          std::vector<unsigned char> calcSHA1Digest() throw(IOException);
          void saveTo(const std::string& path) throw(IOException);

      protected:
          std::string path;
          std::string mediaType;
          std::vector<unsigned char> sha1Digest;

    };
}

#endif // !defined(__DOCUMENT_H_INCLUDED__)
