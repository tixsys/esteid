#if !defined(__SIGNATURE_H_INCLUDED__)
#define __SIGNATURE_H_INCLUDED__

#include <string>

#include "io/IOException.h"

namespace digidoc
{
    /**
     * TODO: implement and add description
     */
    class Signature
    {

      public:
          Signature(const std::string& path, const std::string& mediaType);
          std::string getMediaType() const;
          std::string saveToXml() const throw(IOException);

      protected:

      private:

    };
}

#endif // !defined(__SIGNATURE_H_INCLUDED__)
