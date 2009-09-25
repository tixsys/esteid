/*
 * XmlConf.cpp
 */

#include "XmlConf.h"

#include "log.h"
#include "util/File.h"
#include "util/String.h"
#include "xml/conf.hxx"

#include <stdlib.h>//getenv

/**
 * Environment variable name, that is used for loading configuration
 */
const std::string digidoc::XmlConf::CONF_ENV = "BDOCLIB_CONF_XML";

/**
 * Path to default configuration file
 */
const std::string digidoc::XmlConf::DEFAULT_CONF_LOC = "bdoclib.conf";


const std::string digidoc::XmlConf::DIGEST_URI         = "digest.uri";
const std::string digidoc::XmlConf::PKCS11_DRIVER_PATH = "pkcs11.driver.path";
const std::string digidoc::XmlConf::CERT_STORE_PATH    = "cert.store.path";
const std::string digidoc::XmlConf::MANIFEST_XSD_PATH  = "manifest.xsd.path";
const std::string digidoc::XmlConf::XADES_XSD_PATH     = "xades.xsd.path";
const std::string digidoc::XmlConf::DSIG_XSD_PATH      = "dsig.xsd.path";
const std::string digidoc::XmlConf::PROXY_HOST         = "proxy.host";
const std::string digidoc::XmlConf::PROXY_PORT         = "proxy.port";
const std::string digidoc::XmlConf::PROXY_USER         = "proxy.user";
const std::string digidoc::XmlConf::PROXY_PASS         = "proxy.pass";
const std::string digidoc::XmlConf::PKCS12_CERT        = "pkcs12.cert";
const std::string digidoc::XmlConf::PKCS12_PASS        = "pkcs12.pass";


/**
 * Use digidoc::XmlConf as configuration instance.
 */
void digidoc::XmlConf::initialize()
{
    if(!Conf::isInitialized())
    {
        try
        {
            Conf::init(new XmlConf());
        }
        catch(const digidoc::IOException& e)
        {
            ERR("%s\n", e.getMsg().c_str());
            THROW_IOEXCEPTION("Could not initialize XmlConf");
        }
    }
}


/**
 * Tries to initialize XmlConf by using file defined in BDOCLIB_CONF_XML environment variable.
 * If this is undefined, tries to load configuration from running directory file "bdoclib.conf"
 */
digidoc::XmlConf::XmlConf() throw(IOException)
{
    std::string defaultConfLoc("bdoclib.conf");

    char * envLoc = getenv (CONF_ENV.c_str());
    if(envLoc != NULL)
    {
        std::string envConfLoc(envLoc);
        if(util::File::fileExists(envConfLoc))
        {
            init(envConfLoc);
            return;
        }
    }


    if(util::File::fileExists(DEFAULT_CONF_LOC))
    {
        init(DEFAULT_CONF_LOC);
    }
    else
    {
        THROW_IOEXCEPTION("Error loading xml configuration from '%s' env variable or file '%s'",CONF_ENV.c_str(), DEFAULT_CONF_LOC.c_str());
    }
}

/**
 * Initialize xml conf from path
 * @param path to use for initializing conf
 * @return
 */
digidoc::XmlConf::XmlConf(const std::string& path) throw(IOException)
{
    if(util::File::fileExists(path))
    {
        init(path);
    }
    else
    {
        THROW_IOEXCEPTION("Error loading xml configuration from file '%s'", path.c_str());
    }
}

digidoc::XmlConf::~XmlConf()
{
}

/**
 * Load and parse xml from path. Initialize XmlConf member variables from xml.
 * @param path to use for initializing conf
 */
void digidoc::XmlConf::init(const std::string& path) throw(IOException)
{
    DEBUG("digidoc::XmlConf::init(%s)", path.c_str());
    try
    {
        std::auto_ptr< ::Configuration > conf( configuration (path, xml_schema::Flags::dont_initialize));
        Configuration::ParamSequence paramSeq = conf->param();

        for( Configuration::ParamSequence::const_iterator it = paramSeq.begin(); it != paramSeq.end(); it++)
        {
            if(DIGEST_URI.compare(it->name()) == 0)
            {
                digestUri = *it;
            }
            else if(MANIFEST_XSD_PATH.compare(it->name()) == 0)
            {
                manifestXsdPath = *it;
            }
            else if(XADES_XSD_PATH.compare(it->name()) == 0)
            {
                xadesXsdPath = *it;
            }
            else if(DSIG_XSD_PATH.compare(it->name()) == 0)
            {
                dsigXsdPath = *it;
            }
            else if(PKCS11_DRIVER_PATH.compare(it->name()) == 0)
            {
                pkcs11DriverPath = *it;
            }
            else if(CERT_STORE_PATH.compare(it->name()) == 0)
            {
                certStorePath = *it;
            }
            else if(PROXY_HOST.compare(it->name()) == 0)
            {
                proxyHost = *it;
            }
            else if(PROXY_PORT.compare(it->name()) == 0)
            {
                proxyPort = *it;
            }
            else if(PROXY_USER.compare(it->name()) == 0)
            {
                proxyUser = *it;
            }
            else if(PROXY_PASS.compare(it->name()) == 0)
            {
                proxyPass = *it;
            }
            else if(PKCS12_CERT.compare(it->name()) == 0)
            {
                pkcs12Cert = *it;
            }
            else if(PKCS12_PASS.compare(it->name()) == 0)
            {
                pkcs12Pass = *it;
            }
            else
            {
                WARN("Unknown configuration parameter %s", it->name().c_str());
            }
        }

        // the file path in conf is relative to the conf file's location
        std::string conf_fullpath = digidoc::util::File::directory(getenv(CONF_ENV.c_str()));
        if( !conf_fullpath.empty() ) conf_fullpath += "/";
        Configuration::OcspSequence ocspSeq = conf->ocsp();
        for( Configuration::OcspSequence::const_iterator it = ocspSeq.begin(); it != ocspSeq.end(); ++it)
        {
            OCSPConf o;
            o.issuer = it->issuer();
            o.url = it->url();
            o.cert = it->cert();
            o.cert.insert(0, conf_fullpath);
            ocsp.push_back(o);
        }
    }
    catch(const xml_schema::Exception& e)
    {
        std::ostringstream oss;
        oss << e;
        THROW_IOEXCEPTION("Failed to parse configuration: %s", oss.str().c_str());
    }
}

/**
 * Digest method used to calculate digest values in BDOC (e.g. 'http://www.w3.org/2000/09/xmldsig#sha1' for SHA1)
 *
 * For available method URIs see:
 * <li>
 *   <ul><b>W3C XML Encryption Syntax and Processing</b> (10 December 2005) http://www.w3.org/TR/xmlenc-core/</ul>
 *   <ul><b>RFC 4051</b> http://www.ietf.org/rfc/rfc4051.txt</ul>
 * </li>
 *
 * @return
 */
std::string digidoc::XmlConf::getDigestUri() const
{
    return digestUri;
}

std::string digidoc::XmlConf::getManifestXsdPath() const
{
    // the file path in conf is relative to the conf file's location
    std::string confpath(getenv(CONF_ENV.c_str()));
    return digidoc::util::File::fullPathUrl(digidoc::util::File::directory(confpath), manifestXsdPath);
}

std::string digidoc::XmlConf::getXadesXsdPath() const
{
    // the file path in conf is relative to the conf file's location 
    std::string confpath(getenv(CONF_ENV.c_str()));
    return digidoc::util::File::fullPathUrl(digidoc::util::File::directory(confpath), xadesXsdPath);
}

std::string digidoc::XmlConf::getDsigXsdPath() const
{
    // the file path in conf is relative to the conf file's location
    std::string confpath(getenv(CONF_ENV.c_str()));
    return digidoc::util::File::fullPathUrl(digidoc::util::File::directory(confpath), dsigXsdPath);
}

std::string digidoc::XmlConf::getPKCS11DriverPath() const
{
    return pkcs11DriverPath;
}

digidoc::Conf::OCSPConf digidoc::XmlConf::getOCSP(const std::string &issuer) const
{
    int pos = issuer.find( "CN=", 0 ) + 3;
    std::string _issuer = issuer.substr( pos, issuer.find( ",", pos ) - pos );
    for(std::vector<OCSPConf>::const_iterator i = ocsp.begin(); i != ocsp.end(); ++i)
    {
        if(i->issuer == _issuer)
            return *i;
    }
    OCSPConf o;
    return o;
}

std::string digidoc::XmlConf::getCertStorePath() const
{
    // the file path in conf is relative to the conf file's location
    std::string conf_fullpath(getenv(CONF_ENV.c_str()));
    std::string certStoreFullPath(digidoc::util::File::directory(conf_fullpath));
    certStoreFullPath.append("/" + certStorePath);
    return certStoreFullPath;
}

std::string digidoc::XmlConf::getProxyHost() const
{
    return proxyHost;
}

std::string digidoc::XmlConf::getProxyPort() const
{
    return proxyPort;
}

std::string digidoc::XmlConf::getProxyUser() const
{
    return proxyUser;
}

std::string digidoc::XmlConf::getProxyPass() const
{
    return proxyPass;
}

std::string digidoc::XmlConf::getPKCS12Cert() const
{
    return pkcs12Cert;
}

std::string digidoc::XmlConf::getPKCS12Pass() const
{
    return pkcs12Pass;
}

void digidoc::XmlConf::setProxyHost( const std::string &host )
{
    proxyHost = host;
}

void digidoc::XmlConf::setProxyPort( const std::string &port )
{
    proxyPort = port;
}

void digidoc::XmlConf::setProxyUser( const std::string &user )
{
    proxyUser = user;
}

void digidoc::XmlConf::setProxyPass( const std::string &pass )
{
    proxyPass = pass;
}

void digidoc::XmlConf::setPKCS12Cert( const std::string &cert )
{
    pkcs12Cert = cert;
}

void digidoc::XmlConf::setPKCS12Pass( const std::string &pass )
{
    pkcs12Pass = pass;
}
