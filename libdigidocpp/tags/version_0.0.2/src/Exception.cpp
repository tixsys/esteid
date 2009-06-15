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
    this->causes.push_back(cause);
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
 * Returns exception cause. Before calling this method check that exception cause exists.
 * If the exception cause does not exist an empty exception is returned.
 *
 * @return returns exception cause if the exception has cause otherwise empty exception
 *         is returned.
 * @see hasCause()
 */
digidoc::Exception digidoc::Exception::getCause() const
{
    if(hasCause())
    {
        return causes[0];
    }
    else
    {
        return Exception("", 0, "");
    }
}
