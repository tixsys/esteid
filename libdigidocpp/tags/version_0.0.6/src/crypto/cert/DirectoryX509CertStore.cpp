#include <openssl/pem.h>

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
 * Currently not implemented. Throws IOException if called.
 *
 * @return
 * throws IOException
 */
X509_STORE* digidoc::DirectoryX509CertStore::getCertStore() const throw(IOException)
{
    THROW_IOEXCEPTION("Not implemented");

    // This method is currently not used, it is probably needed,
    // when SSL connection (HTTPS) is implemented in OCSP request.

/*
    X509_STORE_add_cert(X509_STORE *ctx, X509 *x);
    X509_STORE_add_crl((X509_STORE *ctx, X509_CRL *x);
    X509_STORE_add_lookup(X509_STORE *v, X509_LOOKUP_METHOD *m);
    X509_STORE_free(X509_STORE *v);
    X509_STORE_get_by_subject(X509_STORE_CTX *vs,int type,X509_NAME *name,X509_OBJECT *ret);
    X509_STORE_load_locations(X509_STORE *ctx,const char *file, const char *dir);
    X509_STORE_new();
    X509_STORE_set1_param(X509_STORE *ctx, X509_VERIFY_PARAM *pm);
    X509_STORE_set_default_paths(X509_STORE *ctx);
    X509_STORE_set_depth(X509_STORE *store, int depth);
    X509_STORE_set_flags(X509_STORE *ctx, unsigned long flags);
    X509_STORE_set_purpose(X509_STORE *ctx, int purpose);
    X509_STORE_set_trust(X509_STORE *ctx, int trust);
*/
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
