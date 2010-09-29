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

#if !defined(__EXCEPTION_H_INCLUDED__)
#define __EXCEPTION_H_INCLUDED__

#include "Exports.h"

#include <string>
#include <vector>

namespace digidoc
{
    /**
     * Base exception class of the digidoc implementation. Digidoc library
     * should never throw exceptions other type than instance of this class
     * or class inherited from this class like IOException or BDocException.
     *
     * @author Janari Põld
     */
    class EXP_DIGIDOC Exception
    {

      public:
          /**
           * Exception code
           */
          enum ExceptionCode {
              CertificateIssuerMissing = 10,
              CertificateRevoked       = 5,
              CertificateUnknown       = 6,
              NoException              = 0,
              OCSPResponderMissing     = 8,
              OCSPCertMissing          = 9,
              OCSPTimeSlot             = 7,
              OCSPRequestUnauthorized  = 11,
              PINCanceled              = 2,
              PINFailed                = 4,
              PINIncorrect             = 1,
              PINLocked                = 3,
          };
          typedef std::vector<Exception> Causes;

          Exception(const std::string& file, int line, const std::string& msg);
          Exception(const std::string& file, int line, const std::string& msg, const Exception& cause);
          ExceptionCode code() const;
          std::string getMsg() const;
          bool hasCause() const;
          Causes getCauses() const;
          void addCause(const Exception& cause);
          void setCode( ExceptionCode Code );

      protected:
          std::string file;
          int line;
          std::string msg;
          Causes causes;
          ExceptionCode m_code;

    };
}

#endif // !defined(__EXCEPTION_H_INCLUDED__)
