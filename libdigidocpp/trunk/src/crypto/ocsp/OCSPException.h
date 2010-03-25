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

#if !defined(__OCSPEXCEPTION_H_INCLUDED__)
#define __OCSPEXCEPTION_H_INCLUDED__

#include "../../Exception.h"
#include "../../util/String.h"

namespace digidoc
{
    /**
     * OCSP exception implementation. Thrown if OCSP response is not valid or
     * OCSP response status code is not Successful (0x00). OCSP status code can be
     * accessed with method <code>getResponseStatusMessage()</code>. For example
     * if the status code is 0x03 (TryLater) the OCSP request can be be made
     * again (e.g. the OCSP server could be busy at the time).
     *
     * @author Janari Põld
     */
    class OCSPException : public Exception
    {

      public:
          OCSPException(const std::string& file, int line, const std::string& msg, int responseStatusCode);
          OCSPException(const std::string& file, int line, const std::string& msg, const Exception& cause, int responseStatusCode);
          int getResponseStatusCode();
          std::string getResponseStatusMessage();
          static std::string toResponseStatusMessage(int responseStatusCode);

      private:
          int responseStatusCode;

    };
}

#define THROW_OCSPEXCEPTION(rsc, ...) do {std::string _msg(util::String::format(__VA_ARGS__)); ERR(_msg.c_str()); if(fix::always()) throw OCSPException(__FILE__, __LINE__, _msg, rsc);} while(fix::never())

#endif // !defined(__OCSPEXCEPTION_H_INCLUDED__)
