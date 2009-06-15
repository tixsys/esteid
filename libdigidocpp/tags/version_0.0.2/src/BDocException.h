#if !defined(__BDOCEXCEPTION_H_INCLUDED__)
#define __BDOCEXCEPTION_H_INCLUDED__

#include <string>

#include "Exception.h"

namespace digidoc
{
    /**
     * BDOC exception implementation. Thrown if the BDOC is in incorrect format or
     * is used incorrectly.
     *
     * @author Janari Põld
     */
    class BDocException : public Exception
    {

      public:
          BDocException(const std::string& file, int line, const std::string& msg);
          BDocException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_BDOCEXCEPTION(...) do {std::string msg(util::String::format(__VA_ARGS__)); ERROR(msg.c_str()) throw BDocException(__FILE__, __LINE__, msg);} while(0);

#endif // !defined(__BDOCEXCEPTION_H_INCLUDED__)
