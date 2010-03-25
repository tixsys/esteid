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

#include <openssl/pem.h>
#include <openssl/err.h>

#include "../../log.h"
#include "../../Conf.h"
#include "../../util/File.h"
#include "../../util/String.h"
#include "DirectoryX509CertStore.h"


/**
 * Gets certificate store path from configuration and loads all certificates
 * found in directory and adds these to the certificate store.
 *
 * @throws IOException exception is throws if the folder does not exist.
 */
digidoc::DirectoryX509CertStore::DirectoryX509CertStore() throw(IOException)
{
    loadCerts(Conf::getInstance()->getCertStorePath());
}

/**
 * Load all certificates found in directory and adds these to the cert store.
 *
 * @param path path to X.509 certificates in PEM format.
 * @throws IOException exception is throws if the folder does not exist.
 */
digidoc::DirectoryX509CertStore::DirectoryX509CertStore(const std::string& path) throw(IOException)
{
    loadCerts(path);
}

/**
 * Release all certificates.
 */
digidoc::DirectoryX509CertStore::~DirectoryX509CertStore()
{
    for(std::vector<X509*>::const_iterator iter = certs.begin(); iter != certs.end(); iter++)
    {
        X509_free(*iter);
    }
}

/**
 * Load all certificates found in directory and adds these to the cert store.
 *
 * @param path path to X.509 certificates in PEM format.
 * @throws IOException exception is throws if the folder does not exist.
 */
void digidoc::DirectoryX509CertStore::loadCerts(const std::string& path) throw(IOException)
{
    if(!util::File::directoryExists(path))
    {
        THROW_IOEXCEPTION("Directory %s does not exists, can not load cert store.", path.c_str());
    }

    std::vector<std::string> files = util::File::listFiles(path);
    for(std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
    {
        try
        {
            certs.push_back(X509Cert::loadX509(*iter));
        }
        catch(const IOException& e)
        {
            // FIXME: throw exception if failed to load cert or just ignore these files?
            WARN(e.getMsg().c_str());
        }
    }

    INFO("Loaded %d certificates into certificate store.", certs.size());
}

/**
 * Return X509_STORE containing all certs loaded from directory
 * @return X509_STORE cointaining copy of all certs in store. Must be freed using X509_STORE_free() or X509_STORE_scope macro
 * throws IOException
 */
X509_STORE* digidoc::DirectoryX509CertStore::getCertStore() const throw(IOException)
{
    X509_STORE* cert_store = X509_STORE_new();

    if(cert_store == NULL)
    {
        THROW_IOEXCEPTION("Failed to create X509_STORE");
    }

    for(std::vector<X509*>::const_iterator iter = certs.begin(); iter != certs.end(); iter++)
    {
        X509* x = X509Cert::copyX509(*iter);

        if(!X509_STORE_add_cert(cert_store, x))
        {
            WARN("Cant add cert %ld to X509_STORE, %s", ASN1_INTEGER_get(X509_get_serialNumber(*iter)), ERR_reason_error_string(ERR_get_error()));
        }
    }

    return cert_store;
}

/**
 * Searches certificate by serial number and returns a copy of it if found.
 * If not found returns <code>NULL</code>.
 * NB! The returned certificate must be freed with OpenSSL function X509_free(X509* cert)
 * or X509_scope(X509**) macro.
 *
 * @param certSerial certificate serial number.
 * @return returns copy of found certificate or <code>NULL</code> if certificate was not found.
 * @throws IOException exception is thrown if copying certificate failed.
 */
X509* digidoc::DirectoryX509CertStore::getCert(long certSerial) const throw(IOException)
{
    for(std::vector<X509*>::const_iterator iter = certs.begin(); iter != certs.end(); iter++)
    {
        if(ASN1_INTEGER_get(X509_get_serialNumber(*iter)) == certSerial)
        {
            return X509Cert::copyX509(*iter);
        }
    }

    return NULL;
}

/**
 * Searches certificate by subject and returns a copy of it if found.
 * If not found returns <code>NULL</code>.
 * NB! The returned certificate must be freed with OpenSSL function X509_free(X509* cert).
 *
 * @param subject certificate subject.
 * @return returns copy of found certificate or <code>NULL</code> if certificate was not found.
 * @throws IOException exception is thrown if copying certificate failed.
 */
X509* digidoc::DirectoryX509CertStore::getCert(const X509_NAME& subject) const throw(IOException)
{
    for(std::vector<X509*>::const_iterator iter = certs.begin(); iter != certs.end(); iter++)
    {
        if(X509_NAME_cmp(X509_get_subject_name(*iter), &subject) == 0)
        {
            //DEBUG("Found Cert with serial %ld", ASN1_INTEGER_get(X509_get_serialNumber(*iter)));
            return X509Cert::copyX509(*iter);
        }
    }

    return NULL;
}
