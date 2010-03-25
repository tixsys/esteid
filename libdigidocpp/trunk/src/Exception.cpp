/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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

/**
 * @return return exception code
 */
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

/**
 * @param code set exception code
 */
void Exception::setCode( ExceptionCode code ) { m_code = code; }
