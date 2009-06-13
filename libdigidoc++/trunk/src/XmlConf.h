/*
 * XmlConf.h
 *
 *  Created on: 10/02/2009
 *      Author: janno
 */

#ifndef XMLCONF_H_
#define XMLCONF_H_

#include "Conf.h"
#include "io/IOException.h"

namespace digidoc
{

class EXP_DIGIDOC XmlConf: public digidoc::Conf
{
public:
    static void initialize();
    XmlConf() throw(IOException);
    XmlConf(const std::string& path) throw(IOException);
    virtual ~XmlConf();

    virtual std::string getDigestUri() const;

    virtual std::string getManifestXsdPath() const;
    virtual std::string getXadesXsdPath() const;
    virtual std::string getDsigXsdPath() const;
    virtual std::string getPKCS11DriverPath() const;
    virtual std::string getOCSPUrl() const;
    virtual std::string getOCSPCertPath() const;
    virtual std::string getCertStorePath() const;
    virtual std::string getProxyHost() const;
    virtual std::string getProxyPort() const;

    virtual void setProxyHost( const std::string & );
    virtual void setProxyPort( const std::string & );

    static const std::string CONF_ENV;
    static const std::string DEFAULT_CONF_LOC;


private:
    void init(const std::string& path) throw(IOException);

    std::string digestUri;
    std::string pkcs11DriverPath;
    std::string ocspUrl;
    std::string ocspCertsFile;
    std::string certStorePath;
    std::string manifestXsdPath;
    std::string xadesXsdPath;
    std::string dsigXsdPath;
    std::string proxyHost;
    std::string proxyPort;

    static const std::string DIGEST_URI;
    static const std::string PKCS11_DRIVER_PATH;
    static const std::string OCSP_URL;
    static const std::string OCSP_CERTS_FILE;
    static const std::string CERT_STORE_PATH;
    static const std::string MANIFEST_XSD_PATH;
    static const std::string XADES_XSD_PATH;
    static const std::string DSIG_XSD_PATH;
    static const std::string PROXY_HOST;
    static const std::string PROXY_PORT;
};

}

#endif /* XMLCONF_H_ */
