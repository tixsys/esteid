#if !defined(__SIGNATUREEPTION_H_INCLUDED__)
#define __SIGNATUREEXCEPTION_H_INCLUDED__

#include <string>

#include "Exception.h"

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

#define THROW_SIGNATUREEXCEPTION(...) do {std::string msg(util::String::format(__VA_ARGS__)); ERROR(msg.c_str()) throw SignatureException(__FILE__, __LINE__, msg);} while(0);

#endif // !defined(__SIGNATUREEXCEPTION_H_INCLUDED__)
