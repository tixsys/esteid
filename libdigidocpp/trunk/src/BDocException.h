#if !defined(__BDOCEXCEPTION_H_INCLUDED__)
#define __BDOCEXCEPTION_H_INCLUDED__

#include "Exception.h"
#include "util/String.h"

namespace digidoc
{
    /**
     * BDOC exception implementation. Thrown if the BDOC is in incorrect format or
     * is used incorrectly.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC BDocException : public Exception
    {

      public:
          BDocException(const std::string& file, int line, const std::string& msg);
          BDocException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_BDOCEXCEPTION(...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw BDocException(__FILE__, __LINE__, _msg);} while(fix::never())
#define THROW_BDOCEXCEPTION_CAUSE(_cause, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw BDocException(__FILE__, __LINE__, _msg, _cause);} while(fix::never())

#endif // !defined(__BDOCEXCEPTION_H_INCLUDED__)
