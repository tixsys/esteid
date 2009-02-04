#include "../../log.h"
#include "../../util/String.h"
#include "EstEIDSigner.h"


/**
 * Initializes base class PKCS11Signer.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDSigner::EstEIDSigner(const std::string& driver) throw(SignException)
 : PKCS11Signer(driver)
{
}

/**
 *
 */
digidoc::EstEIDSigner::~EstEIDSigner()
{
}

/**
 * Implements signing certificate selector for EstEID ID-Cards. Select signing
 * certificate, which has label 'Allkirjastamine'.
 *
 * @param certificates list of certificates to choose from. List of all certificates
 *        found ID-card.
 * @return should return the selected certificate.
 * @throws SignException throws exception if no suitable certificate was found.
 */
digidoc::PKCS11Signer::PKCS11Cert digidoc::EstEIDSigner::selectSigningCertificate(
        std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException)
{
    // Find EstEID signing certificate (PKCS11 cert label is string 'Allkirjastamine').
    for(std::vector<PKCS11Signer::PKCS11Cert>::const_iterator iter = certificates.begin(); iter != certificates.end(); iter++)
    {
        if(iter->label == std::string("Allkirjastamine"))
        {
            return *iter;
        }
    }

    THROW_SIGNEXCEPTION("Could not find certificate with label 'Allkirjastamine'.");
}
