/*
 * XmlConf.h
 *
 *  Created on: 10/02/2009
 *      Author: janno
 */

#ifndef XMLCONF_H_
#define XMLCONF_H_

#include "Conf.h"
#include "xml/conf.hxx"

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
    virtual OCSPConf getOCSP(const std::string &issuer) const;
    virtual std::string getCertStorePath() const;
    virtual std::string getProxyHost() const;
    virtual std::string getProxyPort() const;
    virtual std::string getProxyUser() const;
    virtual std::string getProxyPass() const;
    virtual std::string getPKCS12Cert() const;
    virtual std::string getPKCS12Pass() const;

    virtual std::string getUserConfDir() const;
    static std::string getDefaultConfDir();

    virtual void setProxyHost( const std::string &host ) throw(IOException);
    virtual void setProxyPort( const std::string &port ) throw(IOException);
    virtual void setProxyUser( const std::string &user ) throw(IOException);
    virtual void setProxyPass( const std::string &pass ) throw(IOException);
    virtual void setPKCS12Cert( const std::string &cert ) throw(IOException);
    virtual void setPKCS12Pass( const std::string &pass ) throw(IOException);

    virtual void setOCSP(const std::string &issuer, const std::string &url, const std::string &cert) throw(IOException);



private:
    void init(const std::string& path) throw(IOException);

    void setUserConf(const std::string &paramName, const std::string &value) throw(IOException);
    void setUserOCSP(const Conf::OCSPConf &ocspData) throw(IOException);
    void serializeUserConf(const ::Configuration &pConf) throw(IOException);
    static std::string getConfSchemaLocation() throw(IOException);
    static void setDefaultConfPath() throw(IOException);
    static void setUserConfPath();

    static std::string DEFAULT_CONF_LOC;
    static std::string USER_CONF_LOC;

    std::string digestUri;
    std::string pkcs11DriverPath;
    std::string certStorePath;
    std::string manifestXsdPath;
    std::string xadesXsdPath;
    std::string dsigXsdPath;
    std::string proxyHost;
    std::string proxyPort;
    std::string proxyUser;
    std::string proxyPass;
    std::string pkcs12Cert;
    std::string pkcs12Pass;
    std::vector<OCSPConf> ocsp;


    static const std::string DIGEST_URI;
    static const std::string PKCS11_DRIVER_PATH;
    static const std::string CERT_STORE_PATH;
    static const std::string MANIFEST_XSD_PATH;
    static const std::string XADES_XSD_PATH;
    static const std::string DSIG_XSD_PATH;
    static const std::string PROXY_HOST;
    static const std::string PROXY_PORT;
    static const std::string PROXY_USER;
    static const std::string PROXY_PASS;
    static const std::string PKCS12_CERT;
    static const std::string PKCS12_PASS;
};

}

#endif /* XMLCONF_H_ */
