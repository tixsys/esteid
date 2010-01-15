#if !defined(__EXCEPTION_H_INCLUDED__)
#define __EXCEPTION_H_INCLUDED__

#include "Exports.h"

#include <string>
#include <vector>

namespace digidoc
{
    /**
     * Base exception class of the digidoc implementation. Digidoc library
     * should never throw exceptions other type than instance of this class
     * or class inherited from this class like IOException or BDocException.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC Exception
    {

      public:
          enum ExceptionCode {
              CertificateIssuerMissing = 10,
              CertificateRevoked       = 5,
              CertificateUnknown       = 6,
              NoException              = 0,
              OCSPResponderMissing     = 8,
              OCSPCertMissing          = 9,
              OCSPTimeSlot             = 7,
              OCSPRequestUnauthorized  = 11,
              PINCanceled              = 2,
              PINFailed                = 4,
              PINIncorrect             = 1,
              PINLocked                = 3,
          };
          typedef std::vector<Exception> Causes;

          Exception(const std::string& file, int line, const std::string& msg);
          Exception(const std::string& file, int line, const std::string& msg, const Exception& cause);
          ExceptionCode code() const;
          std::string getMsg() const;
          bool hasCause() const;
          Causes getCauses() const;
          void addCause(const Exception& cause);
          void setCode( ExceptionCode Code );

      protected:
          std::string file;
          int line;
          std::string msg;
          Causes causes;
          ExceptionCode m_code;

    };
}

#endif // !defined(__EXCEPTION_H_INCLUDED__)