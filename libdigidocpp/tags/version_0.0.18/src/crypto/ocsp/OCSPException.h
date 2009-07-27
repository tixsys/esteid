#if !defined(__OCSPEXCEPTION_H_INCLUDED__)
#define __OCSPEXCEPTION_H_INCLUDED__

#include <string>

#include "../../Exception.h"

namespace digidoc
{
    /**
     * OCSP exception implementation. Thrown if OCSP response is not valid or
     * OCSP response status code is not Successful (0x00). OCSP status code can be
     * accessed with method <code>getResponseStatusMessage()</code>. For example
     * if the status code is 0x03 (TryLater) the OCSP request can be be made
     * again (e.g. the OCSP server could be busy at the time).
     *
     * @author Janari Põld
     */
    class OCSPException : public Exception
    {

      public:
          OCSPException(const std::string& file, int line, const std::string& msg, int responseStatusCode);
          OCSPException(const std::string& file, int line, const std::string& msg, const Exception& cause, int responseStatusCode);
          int getResponseStatusCode();
          std::string getResponseStatusMessage();
          static std::string toResponseStatusMessage(int responseStatusCode);

      private:
          int responseStatusCode;

    };
}

#define THROW_OCSPEXCEPTION(rsc, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw OCSPException(__FILE__, __LINE__, _msg, rsc);} while(fix::never())

#endif // !defined(__OCSPEXCEPTION_H_INCLUDED__)
