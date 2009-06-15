#include <openssl/ocsp.h>

#include "OCSPException.h"


/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 * @param responseStatusCode OCSP response status code.
 * @see Exception::Exception(const std::string& file, int line, const std::string& msg)
 */
digidoc::OCSPException::OCSPException(const std::string& file, int line, const std::string& msg, int responseStatusCode)
 : Exception(file, line, msg)
 , responseStatusCode(responseStatusCode)
{
}

/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 * @param cause cause of the exception.
 * @param responseStatusCode OCSP response status code.
 * @see Exception::Exception(const std::string& file, int line, const std::string& msg, const Exception& cause)
 * @see Exception::hasCause()
 * @see Exception::getCause()
 */
digidoc::OCSPException::OCSPException(const std::string& file, int line, const std::string& msg, const Exception& cause, int responseStatusCode)
: Exception(file, line, msg, cause)
, responseStatusCode(responseStatusCode)
{
}

/**
 * @return returns OCSP response status code.
 */
int digidoc::OCSPException::getResponseStatusCode()
{
    return responseStatusCode;
}

/**
 * @return returns OCSP response status message.
 */
std::string digidoc::OCSPException::getResponseStatusMessage()
{
    return toResponseStatusMessage(responseStatusCode);
}

/**
 * Converts OCSP response status code to corresponding status message.
 *
 * @param responseStatusCode OCSP response status code.
 * @return OCSP status message.
 */
std::string digidoc::OCSPException::toResponseStatusMessage(int responseStatusCode)
{
    switch(responseStatusCode)
    {
      case OCSP_RESPONSE_STATUS_SUCCESSFUL:       { return "Successful";       } break;
      case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST: { return "MalformedRequest"; } break;
      case OCSP_RESPONSE_STATUS_INTERNALERROR:    { return "InternalError";    } break;
      case OCSP_RESPONSE_STATUS_TRYLATER:         { return "TryLater";         } break;
      case OCSP_RESPONSE_STATUS_SIGREQUIRED:      { return "SigRequired";      } break;
      case OCSP_RESPONSE_STATUS_UNAUTHORIZED:     { return "Unauthorized";     } break;
    }

    return "";
}
