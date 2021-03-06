/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Estonian Informatics Centre
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

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"
#include "X509Certificate.h"

class CertificateAPI : public FB::JSAPIAuto
{
public:
    //CertificateAPI(FB::BrowserHost host, X509Certificate cert);
    CertificateAPI(FB::BrowserHost host, ByteVec bv);
    virtual ~CertificateAPI();

    /** Subject CN */
    std::string get_CN();
    /** Date and time in format dd.mm.yyyy hh:mm:ss */
    std::string get_validFrom();
    /** Date and time in format dd.mm.yyyy hh:mm:ss */
    std::string get_validTo();
    /** Issuer CN */
    std::string get_issuerCN();
    /** security, Non-Repudiation, digital signing etc. */
    std::string get_keyUsage();
    /** Certificate in PEM format */
    std::string get_cert();
    /** A unique serial number for this certificate */
    std::string get_serial();
    /** True if certificate seems to be valid (check against workstation clock) */
    bool get_isValid();

private:
    FB::BrowserHost m_host;

    X509Certificate m_cert;
};
