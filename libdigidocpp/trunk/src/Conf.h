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

#if !defined(__CONF_H_INCLUDED__)
#define __CONF_H_INCLUDED__

#include "io/IOException.h"

namespace digidoc
{
    /**
     * TODO: implement and add description
     */
    class EXP_DIGIDOC Conf
    {

      public:
          struct OCSPConf { std::string issuer, url, cert; };
          virtual ~Conf() {}
          static void init(Conf* conf);
          static bool isInitialized();
          static Conf* getInstance() throw(IOException);
          static void destroy();

          virtual std::string getDigestUri() const = 0;
          virtual std::string getManifestXsdPath() const = 0;
          virtual std::string getXadesXsdPath() const = 0;
          virtual std::string getDsigXsdPath() const = 0;
          virtual std::string getPKCS11DriverPath() const = 0;
          virtual OCSPConf getOCSP(const std::string &issuer) const = 0;
          virtual std::string getCertStorePath() const = 0;
          virtual std::string getProxyHost() const = 0;
          virtual std::string getProxyPort() const = 0;
          virtual std::string getProxyUser() const = 0;
          virtual std::string getProxyPass() const = 0;
          virtual std::string getPKCS12Cert() const = 0;
          virtual std::string getPKCS12Pass() const = 0;

          virtual void setProxyHost( const std::string &host ) throw(IOException) = 0;
          virtual void setProxyPort( const std::string &port ) throw(IOException) = 0;
          virtual void setProxyUser( const std::string &user ) throw(IOException) = 0;
          virtual void setProxyPass( const std::string &pass ) throw(IOException) = 0;
          virtual void setPKCS12Cert( const std::string &cert ) throw(IOException) = 0;
          virtual void setPKCS12Pass( const std::string &pass ) throw(IOException) = 0;

          virtual void setOCSP(const std::string &issuer, const std::string &url, const std::string &cert) throw(IOException) = 0;

// Fixme should be protected
          Conf() {}
      private:
// Fixme should be protected
          Conf( const Conf& conf );
          Conf& operator= ( const Conf& conf );

      private:
          //static std::auto_ptr<Conf> INSTANCE;
          static Conf* INSTANCE;

    };
}

#endif // !defined(__CONF_H_INCLUDED__)
