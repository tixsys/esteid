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

#if !defined(__IOEXCEPTION_H_INCLUDED__)
#define __IOEXCEPTION_H_INCLUDED__

#include "../Exception.h"
#include "../util/String.h"

namespace digidoc
{
    /**
     * IO exception implementation. Thrown if IO operations failed.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC IOException : public Exception
    {

      public:
          IOException(const std::string& file, int line, const std::string& msg);
          IOException(const std::string& file, int line, const std::string& msg, const Exception& cause);

    };
}

#define THROW_IOEXCEPTION(...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw IOException(__FILE__, __LINE__, _msg);} while(fix::never())

#endif // !defined(__IOEXCEPTION_H_INCLUDED__)
