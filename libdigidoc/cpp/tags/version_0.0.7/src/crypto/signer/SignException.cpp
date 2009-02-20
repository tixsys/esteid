#include "SignException.h"


/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 * @see Exception::Exception(const std::string& file, int line, const std::string& msg)
 */
digidoc::SignException::SignException(const std::string& file, int line, const std::string& msg)
 : Exception(file, line, msg)
{
}

/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 * @param cause cause of the exception.
 * @see Exception::Exception(const std::string& file, int line, const std::string& msg, const Exception& cause)
 * @see Exception::hasCause()
 * @see Exception::getCause()
 */
digidoc::SignException::SignException(const std::string& file, int line, const std::string& msg, const Exception& cause)
: Exception(file, line, msg, cause)
{
}
