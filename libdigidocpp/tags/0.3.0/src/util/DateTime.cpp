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



#include "DateTime.h"

#include <sstream>
#include <iomanip>


#include "../log.h"

namespace digidoc
{
    namespace util
    {
        namespace date
        {
            /// Dedicated helper for converting xml-schema-style DateTyme into a Zulu-string.
            ///
            /// @param time GMT time as code-synth xml-schema type.
            /// @return a string format of date-time e.g. "2007-12-25T14:06:01Z".
            std::string xsd2string(const xml_schema::DateTime& time)
            {
                std::stringstream stream;

                stream << std::setfill('0') << std::dec
                    << std::setw(4) << time.year()
                    << "-"
                    << std::setw(2) << time.month()
                    << "-"
                    << std::setw(2) << time.day()
                    << "T"
                    << std::setw(2) << time.hours()
                    << ":"
                    << std::setw(2) << time.minutes()
                    << ":"
                    << std::setw(2) << time.seconds()
                    << "Z"
                    ;

                return stream.str();
            }

            xml_schema::DateTime currentTime()
            {
                std::time_t t;
                time(&t);

                return makeDateTime(t);
            }

            xml_schema::DateTime makeDateTime(const std::time_t& time)
            {
                struct tm *lt = gmtime(&time);

                return makeDateTime(*lt);
            }


            xml_schema::DateTime makeDateTime(const struct tm& lt)
            {
 
                xml_schema::DateTime dateTime( lt.tm_year + 1900
                                             , static_cast<unsigned short>( lt.tm_mon + 1 )
                                             , static_cast<unsigned short>( lt.tm_mday )
                                             , static_cast<unsigned short>( lt.tm_hour )
                                             , static_cast<unsigned short>( lt.tm_min )
                                             , lt.tm_sec
                                             , 0 //zone +0h
                                             , 0 ); //zone +0min

                //DEBUG("%d-%d-%dT%d:%d:%dZ",lt.tm_year + 1900, lt.tm_mday + 1, lt.tm_mday, lt.tm_hour,lt.tm_min, lt.tm_sec);
                /*
                DEBUG("DateTime: %d-%d-%dT%d:%d:%dZ",
                        dateTime.year(),
                        dateTime.month(),
                        dateTime.day(),
                        dateTime.hours(),
                        dateTime.minutes(),
                        dateTime.seconds());
*/
                return dateTime;
            }
        }
    }
}
