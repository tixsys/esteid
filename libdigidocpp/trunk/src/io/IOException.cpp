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

#include "IOException.h"


/**
 * @param file filename, where the exception was thrown.
 * @param line line of the file, where the exception was thrown.
 * @param msg error message.
 * @see Exception::Exception(const std::string& file, int line, const std::string& msg)
 */
digidoc::IOException::IOException(const std::string& file, int line, const std::string& msg)
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
digidoc::IOException::IOException(const std::string& file, int line, const std::string& msg, const Exception& cause)
: Exception(file, line, msg, cause)
{
}
