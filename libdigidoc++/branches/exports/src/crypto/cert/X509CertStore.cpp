#include "X509CertStore.h"

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
digidoc::X509CertStore* digidoc::X509CertStore::getInstance()
{
    return INSTANCE;
}
