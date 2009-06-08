#if !defined(__STRING_H_INCLUDED__)
#define __STRING_H_INCLUDED__

#include <memory>
#include <string>

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
              static std::auto_ptr<char> pointer(const std::string& str);

          private:
              static std::string formatArgList(const char *fmt, va_list args);

        };

    }
}

#endif // !defined(__STRING_H_INCLUDED__)
