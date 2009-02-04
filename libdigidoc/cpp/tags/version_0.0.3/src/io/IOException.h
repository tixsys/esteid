#if !defined(__IOEXCEPTION_H_INCLUDED__)
#define __IOEXCEPTION_H_INCLUDED__

#include <string>

#include "../Exception.h"

namespace digidoc
{
    /**
     * IO exception implementation. Thrown if IO operations failed.
     *
     * @author Janari Põld
     */
    class IOException : public Exception
    {

      public:
          IOException(const std::string& file, int line, const std::string& msg);
          IOException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_IOEXCEPTION(...) do {std::string msg(util::String::format(__VA_ARGS__)); ERR(msg.c_str()); throw IOException(__FILE__, __LINE__, msg);} while(0);

#endif // !defined(__IOEXCEPTION_H_INCLUDED__)
