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

#if !defined(__STRING_H_INCLUDED__)
#define __STRING_H_INCLUDED__

#include <string>
#include <stdarg.h>

namespace digidoc
{
    namespace util
    {

        /**
         * Implements common string operations not provided by standard library.
         *
         * @author Janari Põld
         */
        class String
        {
          public:
              static std::string format(const char *fmt, ...);
              static std::string convertUTF8(const std::string& str_in, bool to_UTF);
              static std::string toUriFormat(const std::string& str_in);
#if _WIN32
              static std::string toMultiByte(int format, const std::wstring &in);
              static std::wstring toWideChar(int format, const std::string &in);
#endif

          private:
              static std::string formatArgList(const char *fmt, va_list args);

        };

    }
}

#endif // !defined(__STRING_H_INCLUDED__)
