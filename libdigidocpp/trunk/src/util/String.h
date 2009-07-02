#if !defined(__STRING_H_INCLUDED__)
#define __STRING_H_INCLUDED__

#include <memory>
#include <string>
#include <stdarg.h>
#include "../io/IOException.h"
#include "../BDocException.h"

namespace digidoc
{
    namespace util
    {

        /**
         * Implements common string operations not provided by standard library.
         *
         * @author Janari Põld
         */
        class String
        {
          public:
              static std::string format(const char *fmt, ...);
              static std::string convertUTF8(const std::string& str_in, bool to_UTF);
              static std::string toUriFormat(const std::string& str_in);


          private:
              static const int MAX_LANG_LENGTH = 256; //should be enough to hold the LANG value

              static std::string formatArgList(const char *fmt, va_list args);
              static std::string getSystemEncoding();

        };

    }
}

#endif // !defined(__STRING_H_INCLUDED__)
