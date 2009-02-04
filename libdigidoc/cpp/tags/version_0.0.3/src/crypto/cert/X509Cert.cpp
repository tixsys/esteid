#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "../../log.h"
#include "../../util/String.h"
#include "X509Cert.h"

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

    const unsigned char* pBytes = reinterpret_cast<const unsigned char*>(&bytes[0]);
    d2i_X509(&cert, &pBytes, bytes.size());

    if(cert == NULL)
    {
        THROW_IOEXCEPTION("Failed to parse X509 certificate from bytes given.")
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
 *
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
 *         NB! This struct must be freed using X509_free() function from OpenSSL.
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
        THROW_IOEXCEPTION("Failed to copy X509 certificate: %s", ERR_reason_error_string(ERR_get_error()))
    }

    return copy;
}

/**
 * Parses X.509 PEM formatted certificate from file.
 * NB! This struct must be freed using X509_free() function from OpenSSL.
 *
 * @param path PEM formatted X.509 certificate file path.
 * @return returns certificate parsed from file.
 * @throws IOException throws exception if the file does not contain X.509
 *         PEM formatted certificate.
 */
X509* digidoc::X509Cert::loadX509(const std::string& path) throw(IOException)
{
	// Initialize OpenSSL file.
	BIO* file = BIO_new(BIO_s_file());
    if(file == NULL)
    {
	    THROW_IOEXCEPTION("Failed to open X.509 certificate file '%s': %s",
	    		path.c_str(), ERR_reason_error_string(ERR_get_error()))
	}

	if(BIO_read_filename(file, path.c_str()) <= 0)
	{
	    THROW_IOEXCEPTION("Failed to open X.509 certificate file '%s': %s",
	    		path.c_str(), ERR_reason_error_string(ERR_get_error()))
    }

	// Parse X.509 certificate from file.
    X509* cert = PEM_read_bio_X509(file, NULL, NULL, NULL);
    if(cert == NULL)
    {
    	const char* msg = ERR_reason_error_string(ERR_get_error());
    	BIO_free(file);
    	THROW_IOEXCEPTION("Failed to load X.509 certificate from file '%s': %s", path.c_str(), msg)
    }

    // Release file.
    BIO_free(file);

    return cert;
}

/**
 * Parses X.509 PEM formatted certificates from file. The file can contain multiple
 * certificates. You can just use 'cat' command in linux to add multiple PEM formatted
 * X.509 certificates to one file (e.g. cat <code>cert1.pem cert2.pem > certs.pem</code>).
 * NB! This struct must be freed using sk_X509_free() macro from OpenSSL.
 *
 * @param path PEM formatted X.509 certificates file path.
 * @return returns stack of parsed X.509 certificates.
 * @throws IOException
 */
STACK_OF(X509)* digidoc::X509Cert::loadX509Stack(const std::string& path) throw(IOException)
{
	// Open file, which can contain multiple X.509 certificates.
    FILE* file = fopen(path.c_str(), "r");
    if(file == NULL)
    {
	    THROW_IOEXCEPTION("Failed to open X.509 certificates file '%s'.", path.c_str())
    }

    // Create an empty X.509 stack.
    STACK_OF(X509)* stack = sk_X509_new_null();
	if(stack == NULL)
	{
    	THROW_IOEXCEPTION("Failed to create X.509 certificate stack.")
    }

	// Read certificates info from the file.
	STACK_OF(X509_INFO)* certsInfo = PEM_X509_INFO_read(file, NULL, NULL, NULL);
	if(certsInfo == NULL)
	{
		THROW_IOEXCEPTION("Failed to read X.509 certificates from file '%s'.", path.c_str())
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
    fclose(file);

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
    int bufSize = i2d_X509(cert, NULL);
    if(bufSize < 0)
    {
        THROW_IOEXCEPTION("Failed to encode X509 cert to DER.")
    }

    std::vector<unsigned char> derEncodedX509;
    derEncodedX509.resize(bufSize);

    unsigned char* pBuf = &derEncodedX509[0];
    bufSize = i2d_X509(cert, &pBuf);
    if(bufSize < 0)
    {
        THROW_IOEXCEPTION("Failed to encode X509 cert to DER.")
    }

    return derEncodedX509;
}

std::vector<unsigned char> digidoc::X509Cert::getRsaModulus() const throw(IOException)
{
    EVP_PKEY * pubKey = getPublicKey();
    int mod_len = BN_num_bytes(pubKey->pkey.rsa->n);

    if(mod_len < 0)
    {
        THROW_IOEXCEPTION("Failed to extract RSA modulus.")
    }

    std::vector<unsigned char> rsaModulusBytes(mod_len, 0);

    if( BN_bn2bin(pubKey->pkey.rsa->n, &rsaModulusBytes[0]) > 0)
    {
        return rsaModulusBytes;
    }
    else
    {
        THROW_IOEXCEPTION("Failed to extract RSA modulus.")
    }
}

std::vector<unsigned char> digidoc::X509Cert::getRsaExponent() const throw(IOException)
{
    EVP_PKEY * pubKey = getPublicKey();
    int exp_len = BN_num_bytes(pubKey->pkey.rsa->e);

    if(exp_len < 0)
    {
        THROW_IOEXCEPTION("Failed to extract RSA exponent.")
    }

    std::vector<unsigned char> rsaExponentBytes(exp_len, 0);

    if( BN_bn2bin(pubKey->pkey.rsa->e, &rsaExponentBytes[0]) > 0)
    {
        return rsaExponentBytes;
    }
    else
    {
        THROW_IOEXCEPTION("Failed to extract RSA exponent.")
    }
}

long digidoc::X509Cert::getCertSerial() const
{
    //FIXME: error handling
    ASN1_INTEGER * asnSerialNum = X509_get_serialNumber(cert);
    long certSerial = ASN1_INTEGER_get(asnSerialNum);

    return certSerial;
}


X509_NAME* digidoc::X509Cert::getCertIssuerNameAsn1() const
{
    return X509_get_issuer_name(cert);
}

std::string digidoc::X509Cert::getCertIssuerName() const
{
    //FIXME: error handling
    X509_NAME * x509IssuerName =  X509_get_issuer_name(cert);

    BIO *mem = BIO_new(BIO_s_mem());
    X509_NAME_print_ex(mem, x509IssuerName, 0, XN_FLAG_RFC2253);


    char temp[80];
    int bytes_read;
    std::string issuerName;

    while( (bytes_read = BIO_gets(mem, &temp[0], 80) ) > 0 )
    {
        issuerName.append(temp);
    }

    BIO_free_all(mem);

    DEBUG("Issuername: %s", issuerName.c_str());

    return issuerName;
}

EVP_PKEY * digidoc::X509Cert::getPublicKey() const throw(IOException)
{
    EVP_PKEY * pubKey = X509_get_pubkey(cert);
    if ( (pubKey != NULL) && (pubKey->type == EVP_PKEY_RSA) )
    {
        return pubKey;
    }
    else
    {
        THROW_IOEXCEPTION("Unable to load RSA public Key");
    }
}

void digidoc::X509Cert::printErrorMsg() const
{
    unsigned long errorCode;
    while((errorCode =  ERR_get_error()) != 0)
    {
        char errorMsg[250];
        ERR_error_string(errorCode, &errorMsg[0]);
        ERR("OpenSSL ERROR:%ld %s",errorCode, errorMsg);
    }
}
