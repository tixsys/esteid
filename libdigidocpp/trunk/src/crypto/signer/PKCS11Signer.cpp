#include <memory.h>
#include <map>
#include <libp11.h>
#include <sstream>

#include "../../log.h"
#include "../../Conf.h"
#include "../../util/String.h"
#include "PKCS11Signer.h"

// PKCS#11
#define CKR_OK					(0)
#define CKR_CANCEL				(1)
#define CKR_FUNCTION_CANCELED	(0x50)
#define CKR_PIN_INCORRECT		(0xa0)
#define CKR_PIN_LOCKED			(0xa4)

namespace digidoc
{

struct SignSlot
{
	PKCS11_CERT* certificate;
	PKCS11_SLOT* slot;
	int			number;
};

class PKCS11SignerPrivate
{
public:
	PKCS11SignerPrivate( std::string _driver = "" )
	: driver(_driver)
	, ctx(NULL)
	, slots(NULL)
	, numberOfSlots(0)
	{
		sign.certificate = NULL;
		sign.slot = NULL;
		sign.number = -1;
	};

    digidoc::PKCS11Signer::PKCS11Cert createPKCS11Cert(PKCS11_SLOT* slot, PKCS11_CERT* cert);
    bool checkCert( X509 *cert ) const;

    std::string driver;

    PKCS11_CTX* ctx;
    SignSlot sign;

    PKCS11_SLOT* slots;
    unsigned int numberOfSlots;
};

}

/**
 * Initializes p11 library and loads PKCS #11 driver. Get driver path
 * from configuration.
 *
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::PKCS11Signer::PKCS11Signer() throw(SignException)
 : d(new PKCS11SignerPrivate())
{
    loadDriver();
    DEBUG("PKCS11Signer(driver = '%s'", d->driver.c_str());
}

/**
 * Initializes p11 library and loads PKCS #11 driver.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::PKCS11Signer::PKCS11Signer(const std::string& driver) throw(SignException)
 : d(new PKCS11SignerPrivate(driver))
{
    DEBUG("PKCS11Signer(driver = '%s'", d->driver.c_str());
    loadDriver(d->driver);
}

/**
 * Uninitializes p11 library and releases acquired memory.
 */
digidoc::PKCS11Signer::~PKCS11Signer()
{
    DEBUG("~PKCS11Signer()");
    unloadDriver();
	delete d;
}

void* digidoc::PKCS11Signer::handle() const { return d->ctx; }
int digidoc::PKCS11Signer::slotNumber() const { return d->sign.number; }

void digidoc::PKCS11Signer::unloadDriver()
{
    if(d->ctx == NULL)
        return;

    if(d->slots != NULL)
    {
        // Release all slots.
        PKCS11_release_all_slots(d->ctx, d->slots, d->numberOfSlots);
        d->slots = NULL;
    }

    // Release PKCS #11 context.
    PKCS11_CTX_unload(d->ctx);
    PKCS11_CTX_free(d->ctx);
    d->ctx = NULL;
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
    if(driver.empty())
        THROW_SIGNEXCEPTION("Failed to load driver for PKCS #11 engine: %s.", ERR_reason_error_string(ERR_get_error())); 

    unloadDriver();

    // Create PKCS #11 context.
    d->ctx = PKCS11_CTX_new();

    // Load PKCS #11 driver.
    if(PKCS11_CTX_load(d->ctx, driver.c_str()) != 0)
    {
        PKCS11_CTX_free(d->ctx);
        d->ctx = NULL;
        THROW_SIGNEXCEPTION("Failed to load driver '%s' for PKCS #11 engine: %s",
                driver.c_str(), ERR_reason_error_string(ERR_get_error()));
    }
}

void digidoc::PKCS11Signer::loadDriver() throw(SignException)
{
    if(Conf::getInstance())
        d->driver = Conf::getInstance()->getPKCS11DriverPath();
    loadDriver(d->driver);
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
    if(d->sign.certificate != NULL && d->sign.slot != NULL)
    {
        return d->sign.certificate->x509;
    }

    // Set selected state to 'no certificate selected'.
    d->sign.certificate = NULL;
    d->sign.slot = NULL;
    d->sign.number = -1;
    if(d->slots != NULL)
    {
        // Release all slots.
        PKCS11_release_all_slots(d->ctx, d->slots, d->numberOfSlots);
    }

    // Load all slots.
    if(PKCS11_enumerate_slots(d->ctx, &d->slots, &d->numberOfSlots) != 0)
    {
        THROW_SIGNEXCEPTION("Could not find any ID-Cards in any readers: %s", ERR_reason_error_string(ERR_get_error()));
    }

    // Iterate over all found slots, if the slot has a token, check if the token has any certificates.
    std::vector<PKCS11Cert> certificates;
    std::vector<SignSlot> certSlotMapping;
    int tokenId = 0;
    for(unsigned int i = 0; i < d->numberOfSlots; i++)
    {
        PKCS11_SLOT* slot = d->slots + i;

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
            for(unsigned int j = 0; j < numberOfCerts; j++)
            {
                PKCS11_CERT* cert = certs + j;
                if(!d->checkCert(cert->x509))
                    break;
                SignSlot signSlot = { cert, slot, tokenId };
                certSlotMapping.push_back( signSlot );
                certificates.push_back(d->createPKCS11Cert(slot, cert));
            }
            ++tokenId;
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
    for(std::vector<SignSlot>::const_iterator iter = certSlotMapping.begin(); iter != certSlotMapping.end(); iter++)
    {
        if(iter->certificate->x509 == selectedCert)
        {
            d->sign = *iter;
            break;
        }
    }

    if(d->sign.certificate == NULL || d->sign.slot == NULL)
    {
        THROW_SIGNEXCEPTION("Could not find slot for selected certificate.");
    }

    return d->sign.certificate->x509;
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
    if(d->sign.certificate == NULL || d->sign.slot == NULL)
    {
        THROW_SIGNEXCEPTION("Signing slot or certificate are not selected.");
    }

    // Login if required.
    if(d->sign.slot->token->loginRequired)
    {
        int rv = 0;
        if(d->sign.slot->token->secureLogin)
        {
            showPinpad();
            rv = PKCS11_login(d->sign.slot, 0, NULL);
            hidePinpad();
        }
        else
            rv = PKCS11_login(d->sign.slot, 0, getPin(d->createPKCS11Cert(d->sign.slot, d->sign.certificate)).c_str());
        switch(ERR_GET_REASON(ERR_get_error()))
        {
        case CKR_OK: break;
        case CKR_CANCEL:
        case CKR_FUNCTION_CANCELED:
        {
            SignException e( __FILE__, __LINE__, "PIN acquisition canceled.");
            e.setCode( Exception::PINCanceled );
            throw e;
            break;
        }
        case CKR_PIN_INCORRECT:
        {
            SignException e( __FILE__, __LINE__, "PIN Incorrect" );
            e.setCode( Exception::PINIncorrect );
            throw e;
            break;
        }
        case CKR_PIN_LOCKED:
        {
            SignException e( __FILE__, __LINE__, "PIN Locked" );
            e.setCode( Exception::PINLocked );
            throw e;
            break;
        }
        default:
            std::ostringstream s;
            s << "Failed to login to token '" << d->sign.slot->token->label
                << "': " << ERR_reason_error_string(ERR_get_error());
            SignException e( __FILE__, __LINE__, s.str() );
            e.setCode( Exception::PINFailed );
            throw e;
            break;
        }
    }

    PKCS11_KEY* signKey = PKCS11_find_key(d->sign.certificate);
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

bool digidoc::PKCS11SignerPrivate::checkCert( X509 *cert ) const
{
    if(!cert)
        return false;
    ASN1_BIT_STRING *keyusage = (ASN1_BIT_STRING*)X509_get_ext_d2i(cert, NID_key_usage, NULL, NULL);
    if(!keyusage)
        return false;

    bool ret = false;
    for(int n = 0; n < 9; ++n)
    {
        if(ASN1_BIT_STRING_get_bit(keyusage, n) && n == 1)
        {
            ret = true;
            break;
        }
    }
    ASN1_BIT_STRING_free( keyusage );
    return ret;
}

/**
 * Helper method, creates PKCS11Cert struct. NB! token should not be NULL.
 *
 * @param slot slot to be used to init PKCS11Cert.
 * @param cert cert to be used to init PKCS11Cert.
 * @return returns created PKCS11Cert struct.
 */
digidoc::PKCS11Signer::PKCS11Cert digidoc::PKCS11SignerPrivate::createPKCS11Cert(PKCS11_SLOT* slot, PKCS11_CERT* cert)
{
    digidoc::PKCS11Signer::PKCS11Cert certificate;
    certificate.token.label = slot->token->label;
    certificate.token.manufacturer = slot->token->manufacturer;
    certificate.token.model = slot->token->model;
    certificate.token.serialNr = slot->token->serialnr;
    certificate.label = cert->label;
    certificate.cert = cert->x509;
    return certificate;
}
