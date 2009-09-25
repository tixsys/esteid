#if !defined(__SIGNEXCEPTION_H_INCLUDED__)
#define __SIGNEXCEPTION_H_INCLUDED__

#include "../../Exception.h"
#include "../../util/String.h"

namespace digidoc
{
    class EXP_DIGIDOC SignException : public Exception
    {

      public:
          SignException(const std::string& file, int line, const std::string& msg);
          SignException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_SIGNEXCEPTION(...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw SignException(__FILE__, __LINE__, _msg);} while(fix::never())
#define THROW_SIGNEXCEPTION_CAUSE(_cause, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw SignException(__FILE__, __LINE__, _msg, _cause);} while(fix::never())

#endif // !defined(__SIGNEXCEPTION_H_INCLUDED__)
