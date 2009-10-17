#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "../../log.h"
#include "../../crypto/OpenSSLHelpers.h"
#include "../../util/String.h"
#include "X509Cert.h"
#include "X509CertStore.h"

digidoc::X509Cert::X509Cert(): cert(NULL) {}

/**
 * Creates copy of the X509 certificate.
 *
 * @param cert X509 certificate structure to be wrapped.
 * @throws IOException throws exception if the X509 certificate structure copy fails.
 */
digidoc::X509Cert::X509Cert(X509* cert) throw(IOException)
 : cert(NULL)
{
    this->cert = copyX509(cert);
}

/**
 * Creates X509 certificate from the provider DER encoded bytes.
 *
 * @param bytes X509 certificate in DER encoding.
 * @throws IOException throws exception if X509 certificate parsing failed.
 */
digidoc::X509Cert::X509Cert(std::vector<unsigned char> bytes) throw(IOException)
 : cert(NULL)
{
    if(bytes.empty())
    {
        THROW_IOEXCEPTION("No bytes given to parse X509.");
    }

    // Parse certificate from DER formatted buffer.
    const unsigned char* pBytes = reinterpret_cast<const unsigned char*>(&bytes[0]);
    d2i_X509(&cert, &pBytes, bytes.size());
    if(cert == NULL)
    {
        THROW_IOEXCEPTION("Failed to parse X509 certificate from bytes given: %s", ERR_reason_error_string(ERR_get_error()));
    }
}

/**
 * Copy constructor.
 *
 * @param copy instance of X509Cert class to be copied.
 * @throws IOException throws exception if the X509 cert structure copy fails.
 */
digidoc::X509Cert::X509Cert(const X509Cert& copy) throw(IOException)
 : cert(NULL)
{
    this->cert = copyX509(copy.cert);
}

/**
 * Clean up underlying X509* struct.
 */
digidoc::X509Cert::~X509Cert()
{
    X509_free(cert);
}

/**
 * Assignment operator.
 *
 * @param copy instance of X509Cert class to be assigned.
 * @return copies parameters from the copy instance.
 * @throws IOException throws exception if the X509 cert structure copy fails.
 */
digidoc::X509Cert& digidoc::X509Cert::operator=(const X509Cert& copy) throw(IOException)
{
    this->cert = copyX509(copy.cert);
    return *this;
}

/**
 * @return returns copy of OpenSSL X509 struct pointer that is being wrapped.
 *         NB! This struct must be freed using X509_free() function from OpenSSL or X509_scope(X509**) macro
 * @throws IOException throws exception if the X509 cert structure copy fails.
 */
X509* digidoc::X509Cert::getX509() const throw(IOException)
{
    return copyX509(cert);
}

/**
 * Creates a copy of the X509 certificate.
 *
 * @param cert X509 certificate to be copied.
 * @return returns copy of X509.
 * @throws IOException throws exception if the X509 cert structure copy fails.
 */
X509* digidoc::X509Cert::copyX509(X509* cert) throw(IOException)
{
    X509* copy = X509_dup(cert);
    if(copy == NULL)
    {
        THROW_IOEXCEPTION("Failed to copy X509 certificate: %s", ERR_reason_error_string(ERR_get_error()));
    }

    return copy;
}

/**
 * Parses X.509 PEM formatted certificate from file.
 * NB! This struct must be freed using X509_free() function from OpenSSL or
 * with X509_scope struct.
 *
 * @param path PEM formatted X.509 certificate file path.
 * @return returns certificate parsed from file.
 * @throws IOException throws exception if the file does not contain X.509
 *         PEM formatted certificate.
 */
X509* digidoc::X509Cert::loadX509(const std::string& path) throw(IOException)
{
    // Initialize OpenSSL file.
    BIO* file = BIO_new(BIO_s_file()); BIO_scope fileScope(&file);
    if(file == NULL)
    {
        THROW_IOEXCEPTION("Failed to open X.509 certificate file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    if(BIO_read_filename(file, path.c_str()) <= 0)
    {
        THROW_IOEXCEPTION("Failed to open X.509 certificate file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    // Parse X.509 certificate from file.
    X509* cert = PEM_read_bio_X509(file, NULL, NULL, NULL);
    if(cert == NULL)
    {
        THROW_IOEXCEPTION("Failed to load X.509 certificate from file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    return cert;
}

/**
 * Parses X.509 PEM formatted certificates from file. The file can contain multiple
 * certificates. You can just use 'cat' command in linux to add multiple PEM formatted
 * X.509 certificates to one file (e.g. <code>cat cert1.pem cert2.pem > certs.pem</code>).
 * NB! This struct must be freed using sk_X509_free() function from OpenSSL or
 * with X509Stack_scope struct.
 *
 * @param path PEM formatted X.509 certificates file path.
 * @return returns stack of parsed X.509 certificates.
 * @throws IOException throws exception if the file does not contain X.509
 *         PEM formatted certificate(s).
 */
STACK_OF(X509)* digidoc::X509Cert::loadX509Stack(const std::string& path) throw(IOException)
{
    // Create an empty X.509 stack.
    STACK_OF(X509)* stack = sk_X509_new_null();
    if(stack == NULL)
    {
        THROW_IOEXCEPTION("Failed to create X.509 certificate stack.");
    }

    // Initialize OpenSSL file.
    BIO* file = BIO_new(BIO_s_file()); BIO_scope fileScope(&file);
    if(file == NULL)
    {
        THROW_IOEXCEPTION("Failed to open X.509 certificates file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    // Open file, which can contain multiple X.509 certificates.
    if(BIO_read_filename(file, path.c_str()) <= 0)
    {
        THROW_IOEXCEPTION("Failed to open X.509 certificates file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    // Read certificates info from the file.
    STACK_OF(X509_INFO)* certsInfo = PEM_X509_INFO_read_bio(file, NULL, NULL, NULL);
    if(certsInfo == NULL)
    {
        THROW_IOEXCEPTION("Failed to read X.509 certificates from file '%s': %s",
                path.c_str(), ERR_reason_error_string(ERR_get_error()));
    }

    // Put all found certificates to the stack.
    for(int i = 0; i < sk_X509_INFO_num(certsInfo); i++)
    {
        X509_INFO* xi = sk_X509_INFO_value(certsInfo, i);
        if(xi->x509 != NULL)
        {
            sk_X509_push(stack, xi->x509);
            xi->x509 = NULL;
        }
    }

    // Release resources.
    sk_X509_INFO_pop_free(certsInfo, X509_INFO_free);

    return stack;
}

/**
 * Encodes the X509 certificate using DER encoding.
 *
 * @return returns X509 certificate encoded in DER encoding.
 * @throws IOException throws exception if encoding failed.
 */
std::vector<unsigned char> digidoc::X509Cert::encodeDER() const throw(IOException)
{
    // Get size of the DER encodes certificate.
    int bufSize = i2d_X509(cert, NULL);
    if(bufSize < 0)
    {
        THROW_IOEXCEPTION("Failed to encode X509 cert to DER.");
    }

    // Allocate memory for the DER encoding.
    std::vector<unsigned char> derEncodedX509(bufSize, 0);

    // DER encode X.509 certificate.
    unsigned char* pBuf = &derEncodedX509[0];
    bufSize = i2d_X509(cert, &pBuf);
    if(bufSize < 0)
    {
        THROW_IOEXCEPTION("Failed to encode X509 cert to DER.");
    }

    return derEncodedX509;
}

/**
 * @return returns X.509 certificate serial number.
 * @throws IOException exception is thrown if the serial is incorrect.
 */
long digidoc::X509Cert::getSerial() const throw(IOException)
{
    long serial = ASN1_INTEGER_get(X509_get_serialNumber(cert));
    if(serial <= 0)
    {
        THROW_IOEXCEPTION("Failed to read certificate serial number from X.509 certificate: %s", ERR_reason_error_string(ERR_get_error()));
    }

    return serial;
}

/**
 * @return returns X.509 certificate issuer name. Can also return NULL pointer
 *         if the issuer name is missing.
 */
X509_NAME* digidoc::X509Cert::getIssuerNameAsn1() const
{
    return X509_get_issuer_name(cert);
}

/**
 * Converts X.509 issuer name to string.
 *
 * @return issuer name converted to string.
 * @throws IOException exception is throws if the conversion failed.
 */
std::string digidoc::X509Cert::getIssuerName() const throw(IOException)
{
    X509_NAME* issuerName = X509_get_issuer_name(cert);
    if(issuerName == NULL)
    {
        THROW_IOEXCEPTION("Failed to convert X.509 certificate issuer name: %s", ERR_reason_error_string(ERR_get_error()));
    }

    return toString(issuerName);
}

/**
 * Converts X.509 subject to string.
 *
 * @return issuer name converted to string.
 * @throws IOException exception is throws if the conversion failed.
 */
std::string digidoc::X509Cert::getSubject() const throw(IOException)
{
    X509_NAME* subject = X509_get_subject_name(cert);
    if(subject == NULL)
    {
       THROW_IOEXCEPTION("Failed to convert X.509 certificate subject: %s", ERR_reason_error_string(ERR_get_error()));
    }

    return toString(subject);
}

/**
 * Converts X509_NAME struct to string.
 *
 * @param name X509_NAME struct that is converted to string.
 * @return converted value of X509_NAME.
 * @throws IOException throws exception if conversion failed.
 */
std::string digidoc::X509Cert::toString(X509_NAME* name) throw(IOException)
{
    BIO* mem = BIO_new(BIO_s_mem()); BIO_scope memScope(&mem);
    if(mem == NULL)
    {
        THROW_IOEXCEPTION("Failed to allocate memory for X509_NAME conversion: %s", ERR_reason_error_string(ERR_get_error()));
    }

    // Convert the X509_NAME struct to string.
    if(X509_NAME_print_ex(mem, name, 0, XN_FLAG_RFC2253) < 0)
    {
        THROW_IOEXCEPTION("Failed to convert X509_NAME struct to string: %s", ERR_reason_error_string(ERR_get_error()));
    }

    // Read the converted string from buffer.
    char buf[128];
    int bytesRead;
    std::string str;
    while((bytesRead = BIO_gets(mem, &buf[0], sizeof(buf))) > 0)
    {
        str.append(buf);
    }

    return str;
}

/**
 * @return returns X.509 certificates public key.
 * @throws IOException throws exception if public key is missing.
 */
EVP_PKEY* digidoc::X509Cert::getPublicKey() const throw(IOException)
{
    EVP_PKEY* pubKey = X509_get_pubkey(cert);
    if((pubKey == NULL) || (pubKey->type != EVP_PKEY_RSA))
    {
        THROW_IOEXCEPTION("Unable to load RSA public Key: %s", ERR_reason_error_string(ERR_get_error()));
    }
    return pubKey;
}

/**
 * @return Extracts RSA modulus from X.509 certificate and returns it.
 * @throws IOException throws exception if the RSA modulus extraction failed.
 */
std::vector<unsigned char> digidoc::X509Cert::getRsaModulus() const throw(IOException)
{
    EVP_PKEY* pubKey = getPublicKey();

    // Get size of the RSA modulus.
    int bufSize = BN_num_bytes(pubKey->pkey.rsa->n);

    if(bufSize <= 0)
    {
        THROW_IOEXCEPTION("Failed to extract RSA modulus.");
    }

    // Allocate memory for the RSA modulus.
    std::vector<unsigned char> rsaModulus(bufSize, 0);

    // Extract RSA modulus.
    if(BN_bn2bin(pubKey->pkey.rsa->n, &rsaModulus[0]) <= 0)
    {
        THROW_IOEXCEPTION("Failed to extract RSA modulus.");
    }

	return rsaModulus;
}

/**
 * @return Extracts RSA exponent from X.509 certificate and returns it.
 * @throws IOException throws exception if the RSA exponent extraction failed.
 */
std::vector<unsigned char> digidoc::X509Cert::getRsaExponent() const throw(IOException)
{
    EVP_PKEY* pubKey = getPublicKey();

    // Get size of the RSA exponent.
    int bufSize = BN_num_bytes(pubKey->pkey.rsa->e);

    if(bufSize <= 0)
    {
        THROW_IOEXCEPTION("Failed to extract RSA exponent.");
    }

    // Allocate memory for the RSA exponent.
    std::vector<unsigned char> rsaExponent(bufSize, 0);

    // Extract RSA exponent.
    if(BN_bn2bin(pubKey->pkey.rsa->e, &rsaExponent[0]) <= 0)
    {
        THROW_IOEXCEPTION("Failed to extract RSA exponent.");
    }

	return rsaExponent;
}

bool digidoc::X509Cert::isValid() const throw(IOException)
{
	int notBefore = X509_cmp_current_time(cert->cert_info->validity->notBefore);
	int notAfter = X509_cmp_current_time(cert->cert_info->validity->notAfter);
	if(notBefore == 0 || notAfter == 0)
		THROW_IOEXCEPTION("Failed to validate cert",ERR_reason_error_string(ERR_get_error()));
	return notBefore < 0 && notAfter > 0;
}

/**
 * Check if X509Cert is signed by trusted issuer
 * @param aStore X509_STORE of trusted issuers. If NULL, lib uses X509CertStore::getInstance()->getCertStore()
 * @return 0 or openssl error_code. Get human readable cause with X509_verify_cert_error_string(code)
 * @throw IOException if error
 */
int digidoc::X509Cert::verify(X509_STORE* aStore) const throw(IOException)
{
    X509_STORE* store = aStore;
    X509_STORE** ppStore = NULL;
    if(store == NULL)
    {
         store = digidoc::X509CertStore::getInstance()->getCertStore();
         ppStore = &store;//init ppStore to newly created store, so X509_STORE_scope can free it
    }
    X509_STORE_scope xst(ppStore);

    X509_STORE_CTX *csc = X509_STORE_CTX_new(); X509_STORE_CTX_scope csct(&csc);
    if (csc == NULL)
    {
        THROW_IOEXCEPTION("Failed to create X509_STORE_CTX %s",ERR_reason_error_string(ERR_get_error()));
    }

    X509* x = getX509(); X509_scope xt(&x);
    if(!X509_STORE_CTX_init(csc, store, x, NULL))
    {
        THROW_IOEXCEPTION("Failed to init X509_STORE_CTX %s",ERR_reason_error_string(ERR_get_error()));
    }

    int ok = X509_verify_cert(csc);

    if(!ok)
    {
       int err  = X509_STORE_CTX_get_error(csc);

       X509Cert cause(X509_STORE_CTX_get_current_cert (csc));
       ERR("Unable to verify %s. Cause: %s", cause.getSubject().c_str(), X509_verify_cert_error_string(err));
    }

    return ok;
}
int digidoc::X509Cert::compareIssuerToString(std::string in) const throw(IOException) {
    //FIXME: Actually implement this check

    //retval = X509_NAME_cmp(this->getIssuerNameAsn1(), xn);
    //X509_NAME_free(xn);

    return 0;
}
