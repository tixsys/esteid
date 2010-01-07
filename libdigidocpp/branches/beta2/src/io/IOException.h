#if !defined(__IOEXCEPTION_H_INCLUDED__)
#define __IOEXCEPTION_H_INCLUDED__

#include "../Exception.h"
#include "../util/String.h"

namespace digidoc
{
    /**
     * IO exception implementation. Thrown if IO operations failed.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC IOException : public Exception
    {

      public:
          IOException(const std::string& file, int line, const std::string& msg);
          IOException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_IOEXCEPTION(...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw IOException(__FILE__, __LINE__, _msg);} while(fix::never())

#endif // !defined(__IOEXCEPTION_H_INCLUDED__)
