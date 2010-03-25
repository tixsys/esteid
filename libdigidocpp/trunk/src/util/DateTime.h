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

#if !defined(__DATETIME_H_INCLUDED__)
#define __DATETIME_H_INCLUDED__

#include <time.h>
#include <string>


#include "xml/xmldsig-core-schema.hxx"

namespace digidoc
{
    namespace util
    {

        /// Implements some date and time things specifically dedicated for BDoc.
        ///
        /// @author Andrus Randveer, Vambola Kotkas
        namespace date
        {
            std::string xsd2string(const xml_schema::DateTime& time);

            /**
             *@return current time
             **/
            xml_schema::DateTime currentTime();

            xml_schema::DateTime makeDateTime(const std::time_t& time);
            xml_schema::DateTime makeDateTime(const struct tm& lt);
        }

    }
}

#endif // !defined(__DATETIME_H_INCLUDED__)
