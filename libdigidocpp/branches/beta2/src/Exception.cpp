#include "Exception.h"

using namespace digidoc;

/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 */
Exception::Exception(const std::string& file, int line, const std::string& msg)
 : file(file)
 , line(line)
 , msg(msg)
 , m_code(NoException)
{}

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
Exception::Exception(const std::string& file, int line, const std::string& msg, const Exception& cause)
 : file(file)
 , line(line)
 , msg(msg)
 , m_code(NoException)
{ addCause(cause); }

Exception::ExceptionCode Exception::code() const { return m_code; }

/**
 * @return returns error message.
 */
std::string Exception::getMsg() const { return msg; }

void Exception::addCause(const Exception& cause) { causes.push_back(cause); }


/**
 * @return returns whether the exception has cause.
 */
bool Exception::hasCause() const { return !causes.empty(); }

/**
 * Returns exception causes (other exceptions that caused this exception).
 *
 * @return returns exception causes if the exception has cause otherwise empty collection.
 * @see hasCause()
 */
Exception::Causes Exception::getCauses() const { return causes; }

void Exception::setCode( ExceptionCode code ) { m_code = code; }
