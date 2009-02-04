#include "Exception.h"


/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 */
digidoc::Exception::Exception(const std::string& file, int line, const std::string& msg)
 : file(file)
 , line(line)
 , msg(msg)
{
}

/**
 * Convenience constructor when there is just one cause for this Exception.
 * 
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 * @param cause cause of the exception.
 * @see hasCause()
 * @see getCause()
 */
digidoc::Exception::Exception(const std::string& file, int line, const std::string& msg, const Exception& cause)
 : file(file)
 , line(line)
 , msg(msg)
{
    addCause(cause);
}

/**
 * @return returns error message.
 */
std::string digidoc::Exception::getMsg() const
{
    return msg;
}

/**
 * @return returns whether the exception has cause.
 */
bool digidoc::Exception::hasCause() const
{
    return !causes.empty();
}

/**
 * Returns exception causes (other exceptions that caused this exception).
 *
 * @return returns exception causes if the exception has cause otherwise empty collection.
 * @see hasCause()
 */
digidoc::Exception::Causes digidoc::Exception::getCauses() const
{
    return causes;
}
