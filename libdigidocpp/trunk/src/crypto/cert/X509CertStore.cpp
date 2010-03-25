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

#include "X509CertStore.h"
#include "../../io/IOException.h"
#include "../../log.h"

/**
 * X.509 certificate store implementation.
 */
digidoc::X509CertStore* digidoc::X509CertStore::INSTANCE = NULL;

/**
 * Sets the X.509 certificate store implementation.
 *
 * @param impl X.509 certificate store implementation.
 */
void digidoc::X509CertStore::init(X509CertStore* impl)
{
    INSTANCE = impl;
}

/**
 * Releases the X.509 certificate store implementation.
 */
void digidoc::X509CertStore::destroy()
{
    if(INSTANCE != NULL)
    {
        delete INSTANCE;
        INSTANCE = NULL;
    }
}

/**
 * @return returns the X.509 certificate store implementation.
 */
digidoc::X509CertStore* digidoc::X509CertStore::getInstance() throw(IOException)
{
    if (INSTANCE == NULL)
    {
        THROW_IOEXCEPTION("X509CertStore is not initialized");
    }
    return INSTANCE;
}
