#if !defined(__DOCUMENT_H_INCLUDED__)
#define __DOCUMENT_H_INCLUDED__

#include <string>

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
          void saveTo(const std::string& path) throw(IOException);

      protected:
          std::string path;
          std::string mediaType;

    };
}

#endif // !defined(__DOCUMENT_H_INCLUDED__)
