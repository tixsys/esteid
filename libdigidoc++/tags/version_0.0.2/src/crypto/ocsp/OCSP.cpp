#include "../../log.h"
#include "../../util/String.h"
#include "OCSP.h"


/**
 * Initialize OCSP certificate validator.
 *
 * @param url full OCSP server URL.
 * @throws IOException exception is thrown if provided OCSP URL is in incorrect format.
 * @see setUrl(const std::string& url)
 */
digidoc::OCSP::OCSP(const std::string& url) throw(IOException)
 : host(NULL)
 , port(NULL)
 , path(NULL)
 , ssl(false)
 , skew(5)
 , maxAge(1)
 , connection(NULL)
 , certStore(NULL)
 , ocspCerts(NULL)
 , signCert(NULL)
 , signKey(NULL)
{
    setUrl(url);
}

/**
 * Release acquired private memory.
 */
digidoc::OCSP::~OCSP()
{
    if(connection) { BIO_free_all(connection); }
    if(host) { OPENSSL_free(host); }
    if(port) { OPENSSL_free(port); }
    if(path) { OPENSSL_free(path); }
}

/**
 * Set OCSP connection URL.
 *
 * @param url full OCSP URL (e.g. http://www.openxades.org/cgi-bin/ocsp.cgi).
 * @throws IOException exception is thrown if provided OCSP URL is in incorrect format.
 */
void digidoc::OCSP::setUrl(const std::string& url) throw(IOException)
{
    // Parse OCSP connection URL.
    int sslFlag = 0;
    int result = OCSP_parse_url(util::String::pointer(url).get(), &host, &port, &path, &sslFlag);
    ssl = sslFlag != 0;

    if(result == 0)
    {
        THROW_IOEXCEPTION("Incorrect OCSP URL provided: '%s'.", url.c_str());
    }
}

/**
 * Sets certificate store, which is used to validate OCSP responder and used in HTTPS
 * connection. Default value is NULL, which means certificate store is not used to
 * validate in OCSP responder. In case of HTTPS connection the certification store
 * is mandatory and can not be set to NULL.
 *
 * @param certStore certificate store used to validate OCSP responder and in HTTPS
 *        connection.
 */
void digidoc::OCSP::setCertStore(X509_STORE* certStore)
{
    this->certStore = certStore;
}

/**
 * Sets cerificate stack, which is used to validate OCSP responder. Default value is NULL,
 * which means certification stack is not used to validate OCSP responder.
 *
 * @param ocspCerts certificate stack used to validate OCSP responder.
 */
void digidoc::OCSP::setOCSPCerts(STACK_OF(X509)* ocspCerts)
{
    this->ocspCerts = ocspCerts;
}

/**
 * Sets OCSP request signing certificate. If this certificate is not set the OCSP request
 * is not signed.
 *
 * @param signCert OCSP request signing certificate.
 * @param signKey OCSP request signing certificates private key.
 */
void digidoc::OCSP::setSignCert(X509* signCert, EVP_PKEY* signKey)
{
    this->signCert = signCert;
    this->signKey = signKey;
}

/**
 * @param skew maximum time difference between OCSP server and host computer in seconds.
 *        Default is 5 seconds.
 */
void digidoc::OCSP::setSkew(long skew)
{
    this->skew = skew;
}

/**
 * @param maxAge how old can the precomputed OCSP responses be in seconds. Default is
 *        1 second. The value is validated with OCSP response field producedAt.
 */
void digidoc::OCSP::setMaxAge(long maxAge)
{
    this->maxAge = maxAge;
}

/**
 * Checks the validity of the certificate <code>cert</code> in OCSP server. If the OCSP server
 * certificate should be validated set <code>certStore</code> and/or <code>ocspCerts</code>
 * with corresponding OCSP server certificates.
 *
 * @param cert certificate, which validity is checked in OCSP server.
 * @param issuer certificate issuer certificate chain.
 * @retrn returns certificate status in OCSP server.s
 * @throws IOException throws exception if communication with OCSP server failed (e.g incorrect OCSP URL,
 *         host does not exist, in case of HTTPS connection cert incorrect or missing, could not create
 *         OCSP request, etc).
 * @throws OCSPException throws exception if server responded with invalid response or with
 *         unsuccessful status code (e.g. 0x01: malformedRequest or 0x03: tryLater, etc).
 * @see validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer)
 */
digidoc::OCSP::CertStatus digidoc::OCSP::checkCert(X509* cert, X509* issuer) throw(IOException, OCSPException)
{
    // Connect to OCSP server.
    connect();

    // Create OCSP request.
    OCSP_REQUEST* req = createRequest(cert, issuer); OCSP_REQUEST_scope reqScope(&req);

    // Send the request and get a response.
    OCSP_RESPONSE* resp = sendRequest(req); OCSP_RESPONSE_scope respScope(&resp);

    // Validate response.
    return validateResponse(req, resp, cert, issuer);
}

/**
 * Creates connection to OCSP server based on the <code>url</code> provided.
 *
 * @throws IOException throws exception if connection creation failed.
 */
void digidoc::OCSP::connect() throw(IOException)
{
    if(ssl)
        connectSSL();
    else
        connectNoSSL();
}

/**
 * Creates plain text (not crypted) connection with OCSP server.
 *
 * @throws IOException throws exception if the connection creation failed.
 */
void digidoc::OCSP::connectNoSSL() throw(IOException)
{
    // Release old connection if it exists.
    if(connection) { BIO_free_all(connection); }

    // Establish a connection to the OCSP responder.
    connection = BIO_new_connect(host);
    if(connection == NULL)
    {
        THROW_IOEXCEPTION("Failed to create connection with host: '%s'", host);
    }

    if(BIO_set_conn_port(connection, port) <= 0)
    {
        THROW_IOEXCEPTION("Failed to set port of the connection: %s", port);
    }
}

/**
 * @throws IOException
 */
void digidoc::OCSP::connectSSL() throw(IOException)
{
    // TODO: implement
    THROW_IOEXCEPTION("HTTPS connection is not implemented in OCSP class.")
}

/**
 * Creates OCSP request to check the certificate <code>cert</code> validity.
 *
 * @param cert certificate which validity will be checked.
 * @param issuer issuer of the certificate.
 * @return returns created OCSP request.
 * @throws IOException exception is thrown if the request creation failed.
 */
OCSP_REQUEST* digidoc::OCSP::createRequest(X509* cert, X509* issuer) throw(IOException)
{
    // Create new OCSP request.
    OCSP_REQUEST* req = OCSP_REQUEST_new(); OCSP_REQUEST_scope reqScope(&req);
    if(req == NULL)
    {
        THROW_IOEXCEPTION("Failed to create new OCSP request, out of memory?");
    }

    // Add certificate ID from the certificate being checked.
    OCSP_CERTID* certId = OCSP_cert_to_id(NULL, cert, issuer);
    if(certId == NULL)
    {
        THROW_IOEXCEPTION("Failed to create ID from the certificate being checked.");
    }

    // Add certification ID to the OCSP request.
    if(OCSP_request_add0_id(req, certId) == NULL)
    {
        THROW_IOEXCEPTION("Failed to add certificate ID to OCSP request.");
    }

    // Generate NONCE value to the request.
    if(OCSP_request_add1_nonce(req, NULL, -1) == 0)
    {
        THROW_IOEXCEPTION("Failed to create/add NONCE to OCSP request.");
    }

    // Sign the OCSP request.
    if(signCert && signKey && !OCSP_request_sign(req, signCert, signKey, EVP_sha1(), NULL, 0))
    {
        THROW_IOEXCEPTION("Failed to sign OCSP request.");
    }

    // Remove request pointer from scope, so that it will not be destroyed.
    reqScope.p = NULL;

    return req;
}

/**
 * Sends OCSP request to the server and returns the response got from the server.
 *
 * @param req OCSP request to be sent to the OCSP server.
 * @return returns OCSP response.
 * @throws IOException throws exception if the server failed to accept request or
 *         returned incorrectly formated OCSP response.
 */
OCSP_RESPONSE* digidoc::OCSP::sendRequest(OCSP_REQUEST* req) throw(IOException)
{
    // Send the request and get a response.
    OCSP_RESPONSE* resp = OCSP_sendreq_bio(connection, path, req);
    if(resp == NULL)
    {
        THROW_IOEXCEPTION("Failed to send OCSP request.")
    }

    return resp;
}

/**
 * Validates OCSP response. Checks status code and other fields to match the sent OCSP request.
 * Parses and returns also certificate status code.
 *
 * @param req OCSP request that was sent to the OCSP server.
 * @param resp OCSP response that was received from the OCSP server.
 * @return returns certificate status code.
 * @throws OCSPException throws exception if server responded with invalid response or with
 *         unsuccessful status code (e.g. 0x01: malformedRequest or 0x03: tryLater, etc).
 *         Check also that the response corresponds to the request sent.
 */
digidoc::OCSP::CertStatus digidoc::OCSP::validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer) throw(OCSPException)
{
    // Check OCSP response status code.
    int respStatus = OCSP_response_status(resp);
    if(respStatus != OCSP_RESPONSE_STATUS_SUCCESSFUL)
    {
        THROW_OCSPEXCEPTION(respStatus, "OCSP request failed, response status: %s 0x%02X"
                , OCSPException::toResponseStatusMessage(respStatus).c_str(), respStatus);
    }

    // Verify the OCSP response.
    OCSP_BASICRESP* basic = OCSP_response_get1_basic(resp); OCSP_BASICRESP_scope basicScope(&basic);
    if(basic == NULL)
    {
        THROW_OCSPEXCEPTION(respStatus, "Incorrect OCSP response.")
    }

    // Check NONCE field.
    if(OCSP_check_nonce(req, basic) <= 0)
    {
        THROW_OCSPEXCEPTION(respStatus, "Incorrect NONCE field value.")
    }

    // If ocspCerts or certStore exists verify OCSP responder certificates.
    if(ocspCerts != NULL || certStore != NULL)
    {
        int result = -1;
        if(ocspCerts != NULL)
        {
            result = OCSP_basic_verify(basic, ocspCerts, certStore, OCSP_TRUSTOTHER);
        }

        if(result < 0 && certStore != NULL)
        {
            result = OCSP_basic_verify(basic, NULL, certStore, 0);
        }

        if(result <= 0)
        {
            THROW_OCSPEXCEPTION(respStatus, "OCSP responder certificate not found or not valid.")
        }
    }

    // Check certificate status code.
    OCSP_CERTID* certId = OCSP_cert_to_id(0, cert, issuer); OCSP_CERTID_scope certIdScope(&certId);
    int certStatus = -1; int reason = -1;
    ASN1_GENERALIZEDTIME* producedAt = NULL; ASN1_GENERALIZEDTIME_scope producedAtScope(&producedAt);
    ASN1_GENERALIZEDTIME* thisUpdate = NULL; ASN1_GENERALIZEDTIME_scope thisUpdateScope(&thisUpdate);
    ASN1_GENERALIZEDTIME* nextUpdate = NULL; ASN1_GENERALIZEDTIME_scope nextUpdateScope(&nextUpdate);
    if(!OCSP_resp_find_status(basic, certId, &certStatus, &reason, &producedAt, &thisUpdate, &nextUpdate))
    {
        THROW_OCSPEXCEPTION(respStatus, "Failed to get status code from OCSP response.")
    }

    // Check that response creation time is in valid time slot.
    if(!OCSP_check_validity(thisUpdate, nextUpdate, skew, maxAge))
    {
        THROW_OCSPEXCEPTION(respStatus, "OCSP response not in valid time slot.")
    }

    // Return certificate status.
    switch(certStatus)
    {
      case V_OCSP_CERTSTATUS_GOOD:    { return GOOD;    } break;
      case V_OCSP_CERTSTATUS_REVOKED: { return REVOKED; } break;
      case V_OCSP_CERTSTATUS_UNKNOWN: { return UNKNOWN; } break;
    }

    THROW_OCSPEXCEPTION(respStatus, "OCSP response contains unknown certificate status code.")
}
