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
    else
        return INSTANCE;
}
