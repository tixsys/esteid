#include "OCSP.h"

#include "../../log.h"
#include "../../Conf.h"

#include <openssl/err.h>
#include <openssl/pkcs12.h>

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
 , skew(5*60)
 , maxAge(1*60)
 , connection(NULL)
 , sconnection(NULL)
 , ctx(NULL)
 , ocspCerts(NULL)
 , certStore(NULL)
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
    BIO_free_all(connection);
    SSL_CTX_free(ctx);
}

/**
 * Set OCSP connection URL.
 *
 * @param url full OCSP URL (e.g. http://www.openxades.org/cgi-bin/ocsp.cgi).
 * @throws IOException exception is thrown if provided OCSP URL is in incorrect format.
 */
void digidoc::OCSP::setUrl( const std::string& _url ) throw(IOException)
{
    url = _url;
    // Parse OCSP connection URL.
    int sslFlag = 0;
    int result = OCSP_parse_url(const_cast<char*>(url.c_str()), &host, &port, &path, &sslFlag);
    ssl = sslFlag != 0;

    if(result == 0)
    {
        THROW_IOEXCEPTION("Incorrect OCSP URL provided: '%s'.", url.c_str());
    }
}

/**
 * Sets certificate stack, which is used to validate OCSP responder. Default value is NULL,
 * which means certification stack is not used to validate OCSP responder.
 *
 * @param ocspCerts certificate stack used to validate OCSP responder.
 */
void digidoc::OCSP::setOCSPCerts(STACK_OF(X509)* ocspCerts)
{
    this->ocspCerts = ocspCerts;
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
 *        Default is 5 minutes.
 */
void digidoc::OCSP::setSkew(long skew)
{
    this->skew = skew;
}

/**
 * @param maxAge how old can the precomputed OCSP responses be in seconds. Default is
 *        1 minute. The value is validated with OCSP response field producedAt.
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
 * @param nonce NONCE field value in OCSP request.
 * @return returns certificate status in OCSP server.
 * @throws IOException throws exception if communication with OCSP server failed (e.g incorrect OCSP URL,
 *         host does not exist, in case of HTTPS connection cert incorrect or missing, could not create
 *         OCSP request, etc). X.509 certificate or issuer is <code>NULL</code>.
 * @throws OCSPException throws exception if server responded with invalid response or with
 *         unsuccessful status code (e.g. 0x01: malformedRequest or 0x03: tryLater, etc).
 * @see checkCert(X509* cert, X509* issuer, OCSP_REQUEST** req, OCSP_RESPONSE** resp)
 * @see validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer)
 */
digidoc::OCSP::CertStatus digidoc::OCSP::checkCert(X509* cert, X509* issuer,
        const std::vector<unsigned char>& nonce) throw(IOException, OCSPException)
{
    OCSP_REQUEST* req = NULL; OCSP_REQUEST_scope reqScope(&req);
    OCSP_RESPONSE* resp = NULL; OCSP_RESPONSE_scope respScope(&resp);

    // Check and return certificate status.
    return checkCert(cert, issuer, nonce, &req, &resp);
}

/**
 * Checks the validity of the certificate <code>cert</code> in OCSP server. If the OCSP server
 * certificate should be validated set <code>certStore</code> and/or <code>ocspCerts</code>
 * with corresponding OCSP server certificates. Converts the OCSP response to DER encoding
 * and returns it in output parameter <code>ocspResponseDER</code>
 *
 * @param cert certificate, which validity is checked in OCSP server.
 * @param issuer certificate issuer certificate chain.
 * @param nonce NONCE field value in OCSP request.
 * @param ocspResponseDER output parameter, sets the OCSP response to here in DER format.
 * @param producedAt response production time.
 * @return returns certificate status in OCSP server.
 * @throws IOException throws exception if communication with OCSP server failed (e.g incorrect OCSP URL,
 *         host does not exist, in case of HTTPS connection cert incorrect or missing, could not create
 *         OCSP request, etc). X.509 certificate or issuer is <code>NULL</code>.
 * @throws OCSPException throws exception if server responded with invalid response or with
 *         unsuccessful status code (e.g. 0x01: malformedRequest or 0x03: tryLater, etc).
 * @see checkCert(X509* cert, X509* issuer, OCSP_REQUEST** req, OCSP_RESPONSE** resp)
 * @see validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer)
 */
digidoc::OCSP::CertStatus digidoc::OCSP::checkCert(X509* cert, X509* issuer,
        const std::vector<unsigned char>& nonce, std::vector<unsigned char>& ocspResponseDER,
        tm& producedAt) throw(IOException, OCSPException)
{
    OCSP_REQUEST* req = NULL; OCSP_REQUEST_scope reqScope(&req);
    OCSP_RESPONSE* resp = NULL; OCSP_RESPONSE_scope respScope(&resp);

    // Check and return certificate status.
    CertStatus certStatus = checkCert(cert, issuer, nonce, &req, &resp);


    // Encode the OCSP response in DER encoding.
    int bufSize = i2d_OCSP_RESPONSE(resp, NULL);
    if(bufSize < 0)
    {
        THROW_IOEXCEPTION("Failed to encode OCSP response to DER.");
    }

    ocspResponseDER.resize(bufSize);
    unsigned char* pBuf = &ocspResponseDER[0];
    bufSize = i2d_OCSP_RESPONSE(resp, &pBuf);
    if(bufSize < 0)
    {
        THROW_IOEXCEPTION("Failed to encode OCSP response to DER.");
    }

    // Get 'producedAt' field value from OCSP response.
    OCSP_BASICRESP* basic = OCSP_response_get1_basic(resp); OCSP_BASICRESP_scope basicScope(&basic);
    producedAt = convert(basic->tbsResponseData->producedAt);

    return certStatus;
}

/**
 * Checks the validity of the certificate <code>cert</code> in OCSP server. If the OCSP server
 * certificate should be validated set <code>certStore</code> and/or <code>ocspCerts</code>
 * with corresponding OCSP server certificates.
 *
 * @param cert certificate, which validity is checked in OCSP server.
 * @param issuer certificate issuer certificate chain.
 * @param nonce NONCE field value in OCSP request.
 * @param req output parameter, sets OCSP request to here.
 * @param resp output parameter, sets OCSP response to here.
 * @return returns certificate status in OCSP server.
 * @throws IOException throws exception if communication with OCSP server failed (e.g incorrect OCSP URL,
 *         host does not exist, in case of HTTPS connection cert incorrect or missing, could not create
 *         OCSP request, etc). X.509 certificate or issuer is <code>NULL</code>.
 * @throws OCSPException throws exception if server responded with invalid response or with
 *         unsuccessful status code (e.g. 0x01: malformedRequest or 0x03: tryLater, etc).
 * @see validateResponse(OCSP_REQUEST* req, OCSP_RESPONSE* resp, X509* cert, X509* issuer)
 */
digidoc::OCSP::CertStatus digidoc::OCSP::checkCert(X509* cert, X509* issuer,
        const std::vector<unsigned char>& nonce, OCSP_REQUEST** req, OCSP_RESPONSE** resp)
        throw(IOException, OCSPException)
{
    // Check that X.509 certificate is set.
    if(cert == NULL)
    {
        THROW_IOEXCEPTION("Can not check X.509 certificate, certificate is NULL pointer.");
    }

    // Check that X.509 certificates issuer certificate is set.
    if(issuer == NULL)
    {
        THROW_IOEXCEPTION("Can not check X.509 certificate, issuer certificate is NULL pointer.");
    }

    // Connect to OCSP server.
    connect();

    // Sign request with PKCS#12 certificate
    digidoc::Conf *c = digidoc::Conf::getInstance();
    if(!c->getPKCS12Cert().empty())
    {
        BIO *bio = BIO_new(BIO_s_file());
        BIO_read_filename(bio, c->getPKCS12Cert().c_str());
        PKCS12 *p12 = d2i_PKCS12_bio(bio, NULL);
        BIO_free(bio);
        if(!p12)
            THROW_IOEXCEPTION("Failed to read PKCS12 certificate.");

        X509 *cert;
        EVP_PKEY *key;
        int res = PKCS12_parse(p12, c->getPKCS12Pass().c_str(), &key, &cert, NULL);
        PKCS12_free(p12);
        if(!res)
            THROW_IOEXCEPTION("Failed to parse PKCS12 certificate (%s).", ERR_reason_error_string(ERR_get_error()));

        setSignCert(cert, key);
    }

    // Create OCSP request.
    *req = createRequest(cert, issuer, nonce);

    // Send the request and get a response.
    *resp = sendRequest(*req);

    // Validate response.
    return validateResponse(*req, *resp, cert, issuer);
}

/**
 * Creates connection to OCSP server based on the <code>url</code> provided.
 *
 * @throws IOException throws exception if connection creation failed.
 */
void digidoc::OCSP::connect() throw(IOException)
{
    // Release old connection if it exists.
    BIO_free_all(connection);
    SSL_CTX_free(ctx);
    connection = sconnection = NULL;
    ctx = NULL;

    // proxy host
    digidoc::Conf *c = digidoc::Conf::getInstance();
    if(!c->getProxyHost().empty())
    {
        host = const_cast<char*>(c->getProxyHost().c_str());
        if(!c->getProxyPort().empty())
            port = const_cast<char*>(c->getProxyPort().c_str());
        path = const_cast<char*>(url.c_str());
    }

    // Establish a connection to the OCSP responder.
    connection = BIO_new_connect(host);
    if(connection == NULL)
    {
        THROW_IOEXCEPTION("Failed to create connection with host: '%s'", host);
    }

    if( port != NULL && BIO_set_conn_port(connection, port) <= 0 )
    {
        THROW_IOEXCEPTION("Failed to set port of the connection: %s", port);
    }

    if(ssl)
        connectSSL();

    if ( !BIO_do_connect(connection) )
        THROW_IOEXCEPTION( "Failed to connect to host: '%s'", host );
}

/**
 * Creates plain text (not crypted) connection with OCSP server.
 *
 * @throws IOException throws exception if the connection creation failed.
 */
void digidoc::OCSP::connectNoSSL() throw(IOException)
{
}

/**
 * @throws IOException
 */
void digidoc::OCSP::connectSSL() throw(IOException)
{
	ctx = SSL_CTX_new(SSLv23_client_method());
	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
	sconnection = BIO_new_ssl(ctx, 1);
	connection = BIO_push(sconnection, connection);
}

/**
 * Creates OCSP request to check the certificate <code>cert</code> validity.
 *
 * @param cert certificate which validity will be checked.
 * @param issuer issuer of the certificate.
 * @param nonce NONCE field value in OCSP request.
 * @return returns created OCSP request.
 * @throws IOException exception is thrown if the request creation failed.
 */
OCSP_REQUEST* digidoc::OCSP::createRequest(X509* cert, X509* issuer, const std::vector<unsigned char>& nonce) throw(IOException)
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

    // Add NONCE value to the request.
    if(OCSP_request_add1_nonce(req, const_cast<unsigned char*>(&nonce[0]), nonce.size()) == 0)
    {
        THROW_IOEXCEPTION("Failed to add NONCE to OCSP request.");
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
    OCSP_RESPONSE* resp = 0;

    // Auth proxy
    digidoc::Conf *c = digidoc::Conf::getInstance();
    if(!c->getProxyUser().empty() || !c->getProxyPass().empty())
    {
        BIO *b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO *hash = BIO_push(b64, BIO_new(BIO_s_mem()));
        BIO_printf(hash, "%s:%s", c->getProxyUser().c_str(), c->getProxyPass().c_str());
        BIO_flush(hash);
        char *base64 = 0;
        BIO_get_mem_data(hash, &base64);

        char data[256];
        memset(&data, 0, 256);

        // HACK/FIXME to alter openssl OCSP request http header
        static char post_hdr[] = "%s HTTP/1.0\r\n"
            "Proxy-Authorization: Basic %s\r\n";
        sprintf((char*)&data, post_hdr, path, base64);

        if(!(resp = OCSP_sendreq_bio(connection, (char*)&data, req)))
            THROW_IOEXCEPTION("Failed to send OCSP request.");
    }
    else
    {
        if(!(resp = OCSP_sendreq_bio(connection, path, req)))
            THROW_IOEXCEPTION("Failed to send OCSP request.");
    }

    return resp;
}

/**
 * Validates OCSP response. Checks status code and other fields to match the sent OCSP request.
 * Parses and returns also certificate status code.
 *
 * @param req OCSP request that was sent to the OCSP server.
 * @param resp OCSP response that was received from the OCSP server.
 * @param cert
 * @param issuer
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
        THROW_OCSPEXCEPTION(respStatus, "Incorrect OCSP response.");
    }

    // Check NONCE field.
    if(OCSP_check_nonce(req, basic) <= 0)
    {
        THROW_OCSPEXCEPTION(respStatus, "Incorrect NONCE field value.");
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
            THROW_OCSPEXCEPTION(respStatus, "OCSP responder certificate not found or not valid.");
        }
    }

    // Check certificate status code.
    OCSP_CERTID* certId = OCSP_cert_to_id(0, cert, issuer); OCSP_CERTID_scope certIdScope(&certId);
    int certStatus = -1; int reason = -1;
    ASN1_GENERALIZEDTIME *producedAt = NULL, *thisUpdate = NULL, *nextUpdate = NULL;
    if(!OCSP_resp_find_status(basic, certId, &certStatus, &reason, &producedAt, &thisUpdate, &nextUpdate))
    {
        THROW_OCSPEXCEPTION(respStatus, "Failed to get status code from OCSP response.");
    }

    // Check that response creation time is in valid time slot.
    if(!OCSP_check_validity(thisUpdate, nextUpdate, skew, maxAge))
    {
        OCSPException e( __FILE__, __LINE__, "OCSP response not in valid time slot.", respStatus );
        e.setCode( Exception::OCSPTimeSlot );
        throw e;
    }

    // Return certificate status.
    switch(certStatus)
    {
    default: 
        THROW_OCSPEXCEPTION(respStatus, "OCSP response contains unknown certificate status code.");
    case V_OCSP_CERTSTATUS_UNKNOWN:
        return UNKNOWN;
    case V_OCSP_CERTSTATUS_GOOD:    
        return GOOD;
    case V_OCSP_CERTSTATUS_REVOKED:
        return REVOKED;
    }
}

/**
 * Check that response was signed with trusted OCSP certificate
 * @param ocspResponseDER DER encoded OCSP response bytes
 */
void digidoc::OCSP::verifyResponse(const std::vector<unsigned char>& ocspResponseDER) const throw(IOException)
{
    BIO * bio = BIO_new_mem_buf(const_cast<unsigned char*>(&ocspResponseDER[0]), ocspResponseDER.size()); BIO_scope bioScope(&bio);



    OCSP_RESPONSE * resp = d2i_OCSP_RESPONSE_bio(bio, NULL); OCSP_RESPONSE_scope respScope(&resp);
    OCSP_BASICRESP* basic = OCSP_response_get1_basic(resp); OCSP_BASICRESP_scope basicScope(&basic);


    if(ocspCerts != NULL || certStore != NULL)
    {
        int result = -1;
        if(ocspCerts != NULL)
        {
            result = OCSP_basic_verify(basic, ocspCerts, certStore, OCSP_TRUSTOTHER);
        }

        if(result < 0 && certStore != NULL)
        {
            WARN("OCSP certification not set");
            result = OCSP_basic_verify(basic, NULL, certStore, 0);
        }

        //something went wrong, try to find reason
        if(result <= 0)
        {
            unsigned long errorCode;
            char errorMsg[250];
            while((errorCode =  ERR_get_error()) != 0)
            {
                ERR_error_string(errorCode, &errorMsg[0]);
                ERR("OpenSSL ERROR:%ld %s",errorCode, errorMsg);
            }


            THROW_IOEXCEPTION("OCSP responder certificate not found or not valid.");
        }
    }

}
/**
 * Extract nonce field from ocspResponderDER
 * @param ocspResponseDER
 * @return nonce bytes without any encoding
 */
std::vector<unsigned char> digidoc::OCSP::getNonce(const std::vector<unsigned char>& ocspResponseDER) const
{
    BIO * bio = BIO_new_mem_buf(const_cast<unsigned char*>(&ocspResponseDER[0]), ocspResponseDER.size()); BIO_scope bioScope(&bio);
    OCSP_RESPONSE * resp = d2i_OCSP_RESPONSE_bio(bio, NULL); OCSP_RESPONSE_scope respScope(&resp);
    OCSP_BASICRESP* basic = OCSP_response_get1_basic(resp); OCSP_BASICRESP_scope basicScope(&basic);

    int resp_idx = OCSP_BASICRESP_get_ext_by_NID(basic, NID_id_pkix_OCSP_Nonce, -1);
    X509_EXTENSION* resp_ext = OCSP_BASICRESP_get_ext(basic, resp_idx);// X509_EXTENSION_scope x509ExtScope(&resp_ext);



    int r = i2d_ASN1_OCTET_STRING(resp_ext->value, NULL);


    //int l = M_ASN1_STRING_length(resp_ext->value);
    DEBUG("nonce size: %d", r);
    //DEBUGMEM("nonce raw", resp_ext->value->data, resp_ext->value->length);

    //FIXME: resp_ext->value seems already to contain ASN1_OCTET_STRING
    //FIXME: i2d adds another ASN1_OCTET_STRING container to it
    std::vector<unsigned char> nonceAsn1(r);

    unsigned char * t = &nonceAsn1[0];
    i2d_ASN1_OCTET_STRING(resp_ext->value, &t);

    std::vector<unsigned char> nonce;

    //OpenSSL OCSP created messages NID_id_pkix_OCSP_Nonce field is DER encoded twice, not a problem with java impl
    //XXX: UglyHackTM check if nonceAsn1 contains ASN1_OCTET_STRING
    //XXX: if first 4 bytes seem to be beginning of DER ASN1_OCTET_STRING then remove them
    if(((r > 4) && (nonceAsn1[2] == V_ASN1_OCTET_STRING))
        && (nonceAsn1[3] == r-4))//length is r-4
    {
        DEBUG("Nonce seems to be ASN1_OCTET_STRING, removing DER header bytes");
        nonce.resize(r-4);
        std::copy(&nonceAsn1[4], &nonceAsn1[4]+r-4, nonce.begin());
        return nonce;
    }
    else if(((r > 2) && (nonceAsn1[0] == V_ASN1_OCTET_STRING))
            && (nonceAsn1[1] == r-2))//length is length - DER header len
    {
        nonce.resize(r-2);
        std::copy(&nonceAsn1[2], &nonceAsn1[2]+r-2, nonce.begin());
        return nonce;
    }
    else
    {
        return nonceAsn1;
    }


    //ASN1_TYPE_get_octetstring(resp_ext->value, t, 20);
    //int len = ASN1_STRING_length(resp_ext->value);
    //unsigned char* nonceBytes/* = ASN1_STRING_data(resp_ext->value)*/;
    //_ASN1_bytes(resp_ext->value, &nonceBytes,)
    //ASN1_item_unpack

    //DEBUGMEM("nonce", &nonceAsn1[0], r);*/

    //std::vector<unsigned char> nonce(resp_ext->value->length);
    //std::copy(resp_ext->value->data, resp_ext->value->data+resp_ext->value->length, nonce.begin());
    //return nonceAsn1;
}

/**
 * Extract date time value from ASN1_GENERALIZEDTIME struct.
 *
 * @param asn1Time ASN.1 generalized time struct.
 * @return returned extracted time.
 * @throws IOException exception is throws if the time is in incorrect format.
 */
tm digidoc::OCSP::convert(ASN1_GENERALIZEDTIME* asn1Time) throw(IOException)
{
    char* t = reinterpret_cast<char*>(asn1Time->data);

    if(asn1Time->length < 12)
    {
        THROW_IOEXCEPTION("Date time field value shorter than 12 characters: '%s'", t);
    }

    tm time;
    time.tm_isdst = 0;

    // Accept only GMT time.
    // XXX: What to do, when the time is not in GMT? The time data does not contain
    // DST value and therefore it is not possible to convert it to GMT time.
    if(t[asn1Time->length - 1] != 'Z')
    {
        THROW_IOEXCEPTION("Time value is not in GMT format: '%s'", t);
    }

    for(int i = 0; i< asn1Time->length - 1; i++)
    {
        if ((t[i] > '9') || (t[i] < '0'))
        {
            THROW_IOEXCEPTION("Date time value in incorrect format: '%s'", t);
        }
    }

    // Extract year.
    time.tm_year = ((t[0]-'0')*1000 + (t[1]-'0')*100 + (t[2]-'0')*10 + (t[3]-'0')) - 1900;

    // Extract month.
    time.tm_mon = ((t[4]-'0')*10 + (t[5]-'0')) - 1;
    if((time.tm_mon > 11) || (time.tm_mon < 0))
    {
        THROW_IOEXCEPTION("Month value incorrect: %d", (time.tm_mon + 1));
    }

    // Extract day.
    time.tm_mday = (t[6]-'0')*10 + (t[7]-'0');
    if((time.tm_mday > 31) || (time.tm_mday < 1))
    {
        THROW_IOEXCEPTION("Day value incorrect: %d", time.tm_mday);
    }

    // Extract hour.
    time.tm_hour = (t[8]-'0')*10 + (t[9]-'0');
    if((time.tm_hour > 23) || (time.tm_hour < 0))
    {
        THROW_IOEXCEPTION("Hour value incorrect: %d", time.tm_hour);
    }

    // Extract minutes.
    time.tm_min = (t[10]-'0')*10 + (t[11]-'0');
    if((time.tm_min > 59) || (time.tm_min < 0))
    {
        THROW_IOEXCEPTION("Minutes value incorrect: %d", time.tm_min);
    }

    // Extract seconds.
    time.tm_sec = 0;
    if(asn1Time->length >= 14 && (t[12] >= '0') && (t[12] <= '9') && (t[13] >= '0') && (t[13] <= '9'))
    {
        time.tm_sec = (t[12]-'0')*10 + (t[13]-'0');
        if((time.tm_sec > 59) || (time.tm_sec < 0))
        {
            THROW_IOEXCEPTION("Seconds value incorrect: %d", time.tm_sec);
        }
    }

    return time;
}
