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

#if !defined(__SIGNATUREEXCEPTION_H_INCLUDED__)
#define __SIGNATUREEXCEPTION_H_INCLUDED__

#include "Exception.h"
#include "util/String.h"

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

#define THROW_SIGNATUREEXCEPTION(...) do {std::string _msg(digidoc::util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw SignatureException(__FILE__, __LINE__, _msg);} while(fix::never())
#define THROW_SIGNATUREEXCEPTION_CAUSE(_cause, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw SignatureException(__FILE__, __LINE__, _msg, _cause);} while(fix::never())

#endif // !defined(__SIGNATUREEXCEPTION_H_INCLUDED__)
