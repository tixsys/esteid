#if !defined(__SIGNATUREEXCEPTION_H_INCLUDED__)
#define __SIGNATUREEXCEPTION_H_INCLUDED__

#include "Exception.h"
#include "util/String.h"

namespace digidoc
{
    /**
     * Signature exception implementation. Thrown if the signature is in incorrect format or
     * is used incorrectly.
     *
     * @author Janari Põld
     */
    class SignatureException : public Exception
    {

      public:
          SignatureException(const std::string& file, int line, const std::string& msg);
          SignatureException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_SIGNATUREEXCEPTION(...) do {std::string _msg(digidoc::util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw SignatureException(__FILE__, __LINE__, _msg);} while(fix::never())
#define THROW_SIGNATUREEXCEPTION_CAUSE(_cause, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw SignatureException(__FILE__, __LINE__, _msg, _cause);} while(fix::never())

#endif // !defined(__SIGNATUREEXCEPTION_H_INCLUDED__)