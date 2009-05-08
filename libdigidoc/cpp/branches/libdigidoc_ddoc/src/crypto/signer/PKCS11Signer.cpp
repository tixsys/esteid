#include <memory.h>
#include <map>

#include "../../log.h"
#include "../../Conf.h"
#include "../../util/String.h"
#include "PKCS11Signer.h"


/**
 * Initializes p11 library and loads PKCS #11 driver. Get driver path
 * from configuration.
 *
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::PKCS11Signer::PKCS11Signer() throw(SignException)
 : driver("")
 , ctx(NULL)
 , signCertificate(NULL)
 , signSlot(NULL)
 , slots(NULL)
 , numberOfSlots(0)
{
    DEBUG("PKCS11Signer(driver = '%s'", driver.c_str());
    driver = Conf::getInstance()->getPKCS11DriverPath();
    loadDriver(driver);
}

/**
 * Initializes p11 library and loads PKCS #11 driver.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::PKCS11Signer::PKCS11Signer(const std::string& driver) throw(SignException)
 : driver(driver)
 , ctx(NULL)
 , signCertificate(NULL)
 , signSlot(NULL)
 , slots(NULL)
 , numberOfSlots(0)
{
    DEBUG("PKCS11Signer(driver = '%s'", driver.c_str());
    loadDriver(driver);
}

/**
 * Uninitializes p11 library and releases acquired memory.
 */
digidoc::PKCS11Signer::~PKCS11Signer()
{
    DEBUG("~PKCS11Signer()");
    unloadDriver();
}

void digidoc::PKCS11Signer::unloadDriver()
{
    if(ctx == NULL)
        return;

    if(slots != NULL)
    {
        // Release all slots.
        PKCS11_release_all_slots(ctx, slots, numberOfSlots);
        slots = NULL;
    }

    // Release PKCS #11 context.
    PKCS11_CTX_unload(ctx);
    PKCS11_CTX_free(ctx);
    ctx = NULL;
}

/**
 * Initializes p11 library and loads PKCS #11 driver.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
void digidoc::PKCS11Signer::loadDriver(const std::string& driver) throw(SignException)
{
    // Create PKCS #11 context.
    ctx = PKCS11_CTX_new();

    // Load PKCS #11 driver.
    if(PKCS11_CTX_load(ctx, driver.c_str()) != 0)
    {
        PKCS11_CTX_free(ctx);
        THROW_SIGNEXCEPTION("Failed to load driver '%s' for PKCS #11 engine: %s",
                driver.c_str(), ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Finds all slots connected with the computer, if the slots have tokens, lists all
 * certificates found in token. If there are more that 1 certificate lets the user
 * application select (by calling the <code>selectSignCertificate</code> callback
 * function) the certificate used for signing.
 *
 * @return returns certificate used for signing.
 * @throws throws exception if failed to select the signing certificate. For example
 *         no cards found or card has no certificate.
 */
X509* digidoc::PKCS11Signer::getCert() throw(SignException)
{
    DEBUG("PKCS11Signer::getCert()");

    // If certificate is already selected return it.
    if(signCertificate != NULL && signSlot != NULL)
    {
        return signCertificate->x509;
    }

    // Set selected state to 'no certificate selected'.
    signCertificate = NULL;
    signSlot = NULL;
    if(slots != NULL)
    {
        // Release all slots.
        PKCS11_release_all_slots(ctx, slots, numberOfSlots);
    }

    // Load all slots.
    if(PKCS11_enumerate_slots(ctx, &slots, &numberOfSlots) != 0)
    {
        THROW_SIGNEXCEPTION("Could not find any ID-Cards in any readers: %s", ERR_reason_error_string(ERR_get_error()));
    }

    // Iterate over all found slots, if the slot has a token, check if the token has any certificates.
    std::vector<PKCS11Cert> certificates;
    std::map<PKCS11_CERT*, PKCS11_SLOT*> certSlotMapping;
    for(unsigned int i = 0; i < numberOfSlots; i++)
    {
        PKCS11_SLOT* slot = slots + i;

        if(slot->token != NULL)
        {
            PKCS11_CERT* certs = NULL;
            unsigned int numberOfCerts = 0;
            if(PKCS11_enumerate_certs(slot->token, &certs, &numberOfCerts))
            {
                // XXX: Should this throw an exception instead?
                WARN("Could not get certificates from token '%s'.", slot->token->label);
                continue;
            }

            // If no certificates on token skip the token.
            if(numberOfCerts <= 0)
            {
                continue;
            }

            // List all certificates found on this token.
            for(unsigned int i = 0; i < numberOfCerts; i++)
            {
                PKCS11_CERT* cert = certs + i;
                certSlotMapping[cert] = slot;
                certificates.push_back(createPKCS11Cert(slot, cert));
            }
        }
    }

    if(certificates.size() == 0)
    {
        THROW_SIGNEXCEPTION("No certificates found.");
    }

    // Let the application select the signing certificate.
    X509* selectedCert = selectSigningCertificate(certificates).cert;

    if(selectedCert == NULL)
    {
        THROW_SIGNEXCEPTION("No certificate selected.");
    }

    // Find the corresponding slot and PKCS11 certificate struct.
    for(std::map<PKCS11_CERT*, PKCS11_SLOT*>::const_iterator iter = certSlotMapping.begin(); iter != certSlotMapping.end(); iter++)
    {
        if(iter->first->x509 == selectedCert)
        {
            signCertificate = iter->first;
            signSlot = iter->second;
            break;
        }
    }

    if(signCertificate == NULL || signSlot == NULL)
    {
        THROW_SIGNEXCEPTION("Could not find slot for selected certificate.");
    }

    return signCertificate->x509;
}

/**
 * Signs the digest provided using the selected certificate. If the certificate needs PIN,
 * the PIN is acquired by calling the callback function <code>getPin</code>.
 *
 * @param digest digest, which is being signed.
 * @param signature memory for the signature that is created. Struct parameter <code>length</code>
 *        is set to the actual signature length.
 * @throws SignException throws exception if the signing operation failed.
 */
void digidoc::PKCS11Signer::sign(const Digest& digest, Signature& signature) throw(SignException)
{
    DEBUG("sign(digest = {type=%s,digest=0x%X,length=%d}, signature={signature=0x%X,length=%d})",
            OBJ_nid2sn(digest.type), (unsigned int)digest.digest, digest.length,
            (unsigned int)signature.signature, signature.length);

    // Check that sign slot and certificate are selected.
    if(signCertificate == NULL || signSlot == NULL)
    {
        THROW_SIGNEXCEPTION("Signing slot or certificate are not selected.");
    }

    // Login if required.
    if(signSlot->token->loginRequired)
    {
        // Perform PKCS #11 login.
        if(PKCS11_login(signSlot, 0, getPin(createPKCS11Cert(signSlot, signCertificate)).c_str()) != 0)
        {
            THROW_SIGNEXCEPTION("Failed to login to token '%s': %s", signSlot->token->label,
                    ERR_reason_error_string(ERR_get_error()));
        }
    }

    PKCS11_KEY* signKey = PKCS11_find_key(signCertificate);
    if(signKey == NULL)
    {
        THROW_SIGNEXCEPTION("Could not get key that matches selected certificate.");
    }

    // Sign the digest.
    int result = PKCS11_sign(digest.type, digest.digest, digest.length, signature.signature, &(signature.length), signKey);
    if(result != 1)
    {
        THROW_SIGNEXCEPTION("Failed to sign digest: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Helper method, creates PKCS11Cert struct. NB! token should not be NULL.
 *
 * @param slot slot to be used to init PKCS11Cert.
 * @param cert cert to be used to init PKCS11Cert.
 * @return returns created PKCS11Cert struct.
 */
digidoc::PKCS11Signer::PKCS11Cert digidoc::PKCS11Signer::createPKCS11Cert(PKCS11_SLOT* slot, PKCS11_CERT* cert)
{
    PKCS11Cert certificate;
    certificate.token.label = slot->token->label;
    certificate.token.manufacturer = slot->token->manufacturer;
    certificate.token.model = slot->token->model;
    certificate.token.serialNr = slot->token->serialnr;
    certificate.label = cert->label;
    certificate.cert = cert->x509;
    return certificate;
}
