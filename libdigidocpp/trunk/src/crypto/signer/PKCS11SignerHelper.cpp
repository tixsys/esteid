#include <memory.h>
#include <map>
#include <pkcs11h-core.h>
#include <pkcs11h-certificate.h>
#include <pkcs11h-token.h>
#include <pkcs11h-openssl.h>
#include <openssl/err.h>

#include "../../log.h"
#include "../../Conf.h"
#include "../../util/String.h"
#include "PKCS11Signer.h"

static PKCS11H_BOOL _pkcs11h_hooks_pin_prompt(
	void * const global_data,
	void * const user_data,
	const pkcs11h_token_id_t token,
	const unsigned retry,
	char * const pin,
	const size_t pin_max)
{
	digidoc::PKCS11Signer::PKCS11Cert c;
	c.token.label = token->label;
	c.token.manufacturer = token->manufacturerID;
	c.token.model = token->model;
	c.token.serialNr = token->serialNumber;
	c.cert = (X509*)user_data;
	try
	{
		digidoc::PKCS11Signer *signer = (digidoc::PKCS11Signer*)global_data;
		std::string p = signer->getPin(c);
		strncpy(pin, p.c_str(), pin_max);
		pin[pin_max-1] = '\0';
		return true;
	}
	catch(const digidoc::Exception &e)
	{
		return false;
	}
}


namespace digidoc
{
class PKCS11SignerPrivate
{
public:
	PKCS11SignerPrivate(std::string _driver = "")
	: driver(_driver)
	, signCertificate(NULL)
	, cert(NULL)
	{}

	static PKCS11Signer::PKCS11Cert createPKCS11Cert(pkcs11h_certificate_id_t cert);
    std::string driver;
	pkcs11h_certificate_id_t signCertificate;
	X509 *cert;
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
    DEBUG("PKCS11Signer(driver = '%s'", d->driver.c_str());
    d->driver = Conf::getInstance()->getPKCS11DriverPath();
    loadDriver(d->driver);
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

void digidoc::PKCS11Signer::unloadDriver()
{
	if(d->cert != NULL)
		X509_free(d->cert);
	if(d->signCertificate != NULL)
		pkcs11h_certificate_freeCertificateId(d->signCertificate);
	pkcs11h_removeProvider(d->driver.c_str());
	pkcs11h_terminate();
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
	CK_RV rv = pkcs11h_initialize();
	if(rv != CKR_OK)
		THROW_SIGNEXCEPTION("Failed to init PKCS #11 engine: %s", pkcs11h_getMessage(rv));

	rv = pkcs11h_setPINPromptHook(_pkcs11h_hooks_pin_prompt, (void *const)this);
	if(rv != CKR_OK)
	{
		unloadDriver();
		THROW_SIGNEXCEPTION("Failed to set PKCS #11 engine pin hook: %s", pkcs11h_getMessage(rv));
	}

	rv = pkcs11h_addProvider(d->driver.c_str(), d->driver.c_str(), TRUE,
		PKCS11H_PRIVATEMODE_MASK_AUTO, PKCS11H_SLOTEVENT_METHOD_AUTO, 0, FALSE);
	if(rv != CKR_OK)
	{
		unloadDriver();
		THROW_SIGNEXCEPTION("Failed to load driver '%s' for PKCS #11 engine: %s",
			driver.c_str(), pkcs11h_getMessage(rv));
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
	if(d->cert != NULL)
		return d->cert;

    // Load all slots.
	pkcs11h_certificate_id_list_t issuers, certs;
	CK_RV rv = pkcs11h_certificate_enumCertificateIds(PKCS11H_ENUM_METHOD_CACHE, NULL,
		PKCS11H_PROMPT_MASK_ALLOW_ALL, &issuers, &certs);
	if(rv != CKR_OK || certs == NULL)
		THROW_SIGNEXCEPTION("Could not find any ID-Cards in any readers: %s", pkcs11h_getMessage(rv));

    // Iterate over all found slots, if the slot has a token, check if the token has any certificates.
	std::vector<PKCS11Cert> certificates;
	for(pkcs11h_certificate_id_list_t temp = certs; temp != NULL; temp = temp->next)
		certificates.push_back(PKCS11SignerPrivate::createPKCS11Cert(temp->certificate_id));

    if(certificates.size() == 0)
        THROW_SIGNEXCEPTION("No certificates found.");

    // Let the application select the signing certificate.
	PKCS11Cert selectedCert;
	try
	{
		selectedCert = selectSigningCertificate(certificates);
	}
	catch(const SignException &) {}

	for(int i = 0; i < certificates.size(); ++i)
		X509_free(certificates[i].cert);

	if(selectedCert.label.empty())
        THROW_SIGNEXCEPTION("No certificate selected.");

    // Find the corresponding slot and PKCS11 certificate struct.
	for(pkcs11h_certificate_id_list_t temp = certs; temp != NULL; temp = temp->next)
	{
		if(selectedCert.label.compare(temp->certificate_id->displayName) == 0)
		{
			pkcs11h_certificate_duplicateCertificateId(
				&d->signCertificate, temp->certificate_id);
			break;
		}
	}
	pkcs11h_certificate_freeCertificateIdList(issuers);
	pkcs11h_certificate_freeCertificateIdList(certs);

	if(d->signCertificate == NULL)
		THROW_SIGNEXCEPTION("Could not find sign certificate.");

	// Read sign certificate to X509 objectERR_peek_last_error(void)
	pkcs11h_certificate_t cert;
	rv = pkcs11h_certificate_create(d->signCertificate, NULL, PKCS11H_PROMPT_MASK_ALLOW_ALL, 0, &cert);
	if(rv != CKR_OK)
		THROW_SIGNEXCEPTION("Could not read sign certificate: %s", pkcs11h_getMessage(rv));
	d->cert = pkcs11h_openssl_getX509(cert);
	pkcs11h_certificate_freeCertificate(cert);
	if(d->cert == NULL)
		THROW_SIGNEXCEPTION("Could not read sign certificate.");
	return d->cert;
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

	// Check that sign certificate is selected.
	if(d->signCertificate == NULL)
		THROW_SIGNEXCEPTION("Signing certificate is not selected.");

	pkcs11h_certificate_t cert;
	CK_RV rv = pkcs11h_certificate_create(d->signCertificate, NULL, PKCS11H_PROMPT_MASK_ALLOW_ALL, 0, &cert);
	if(rv != CKR_OK)
		THROW_SIGNEXCEPTION("Could not read sign certificate: %s", pkcs11h_getMessage(rv));
	pkcs11h_certificate_setUserData(cert, (void *const)d->cert);

	pkcs11h_openssl_session_t openssl = pkcs11h_openssl_createSession(cert);
	if(openssl == NULL)
		THROW_SIGNEXCEPTION("Failed to create openssl session.");
	RSA* rsa = pkcs11h_openssl_session_getRSA(openssl);
	if(rsa == NULL)
		THROW_SIGNEXCEPTION("Failed to get openssl RSA object.");

	signature.signature = (unsigned char*)malloc(RSA_size(rsa));

	// Sign the digest.
	int sign = RSA_sign(digest.type, digest.digest, digest.length, signature.signature, &signature.length, rsa);
	pkcs11h_openssl_freeSession(openssl);

	if(sign != 1)
		THROW_SIGNEXCEPTION("Failed to sign RSA signature: %s", ERR_error_string(ERR_peek_last_error(),NULL));
}

/**
 * Helper method, creates PKCS11Cert struct. NB! token should not be NULL.
 *
 * @param slot slot to be used to init PKCS11Cert.
 * @param cert cert to be used to init PKCS11Cert.
 * @return returns created PKCS11Cert struct.
 */
digidoc::PKCS11Signer::PKCS11Cert digidoc::PKCS11SignerPrivate::createPKCS11Cert(pkcs11h_certificate_id_t cert)
{
	digidoc::PKCS11Signer::PKCS11Cert certificate;
	certificate.token.label = cert->token_id->label;
	certificate.token.manufacturer = cert->token_id->manufacturerID;
	certificate.token.model = cert->token_id->model;
	certificate.token.serialNr = cert->token_id->serialNumber;
	certificate.label = cert->displayName;

	pkcs11h_certificate_t cert_object;
	pkcs11h_certificate_create(cert, NULL, PKCS11H_PROMPT_MASK_ALLOW_ALL, 0, &cert_object);
	certificate.cert = pkcs11h_openssl_getX509(cert_object);
	pkcs11h_certificate_freeCertificate(cert_object);

	return certificate;
}
