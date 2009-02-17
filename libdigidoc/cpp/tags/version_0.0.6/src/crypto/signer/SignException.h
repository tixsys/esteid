#if !defined(__SIGNEXCEPTION_H_INCLUDED__)
#define __SIGNEXCEPTION_H_INCLUDED__

#include <string>

#include "../../Exception.h"

namespace digidoc
{
    class SignException : public Exception
    {

      public:
          SignException(const std::string& file, int line, const std::string& msg);
          SignException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_SIGNEXCEPTION(...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); throw SignException(__FILE__, __LINE__, _msg);} while(0)
#define THROW_SIGNEXCEPTION_CAUSE(_cause, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); throw SignException(__FILE__, __LINE__, _msg, _cause);} while(0)

#endif // !defined(__SIGNEXCEPTION_H_INCLUDED__)
