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

#include "ADoc.h"

#include "XmlConf.h"
#include "util/File.h"

#include <xsec/utils/XSECPlatformUtils.hpp>

/**
 * Initializes libraries used in digidoc implementation.
 */
void digidoc::initialize()
{
    // Initialize OpenSSL library.
    SSL_load_error_strings();
    SSL_library_init();
    OPENSSL_config(NULL);

    // Initialize Apache Xerces library.
    xercesc::XMLPlatformUtils::Initialize();

    // Initialize Apache XML Security library.
    XSECPlatformUtils::Initialise();

    //Use Xml based configuration
    XmlConf::initialize();
}

/**
 * Terminates libraries used in digidoc implementation.
 */
void digidoc::terminate()
{
    XmlConf::destroy();

    // Terminate Apache XML Security library.
    XSECPlatformUtils::Terminate();

    // Terminate Apache Xerces library.
    xercesc::XMLPlatformUtils::Terminate();

    // Try to delete temporary files created with the util::File::tempFileName, tempDirectory and createTempDirectory functions.
    util::File::deleteTempFiles();
}
