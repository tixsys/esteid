/*
 * XmlConf.cpp
 */

#include "XmlConf.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <Winreg.h>
#include <direct.h>
#endif

#include "log.h"
#include "util/File.h"
#include "util/String.h"

#include <xsd/cxx/xml/dom/parsing-source.hxx> //for parsing xml data into DOMDocument
#include <stdlib.h>//getenv

#ifdef _WIN32
#ifndef DIGIDOCPP_PATH_REGISTRY_KEY
#define DIGIDOCPP_PATH_REGISTRY_KEY "SOFTWARE\\Estonian ID Card\\digidocpp"
#endif
#endif

/**
 * Path to default configuration files
 */
std::string digidoc::XmlConf::DEFAULT_CONF_LOC = "";
std::string digidoc::XmlConf::USER_CONF_LOC = "";

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
    setDefaultConfPath();
    setUserConfPath();

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
 * Tries to initialize XmlConf by using file defined in DIGIDOCPP_OVERRIDE_CONF environment variable.
 * If this is undefined, tries to load configuration from defined Default and user configuration file
 */
digidoc::XmlConf::XmlConf() throw(IOException)
{

    if (!DEFAULT_CONF_LOC.size())
        THROW_IOEXCEPTION("XmlConf not initialized!");
    if(util::File::fileExists(DEFAULT_CONF_LOC))
        init(DEFAULT_CONF_LOC);
    else
        THROW_IOEXCEPTION("Error loading xml configuration from '%s' file",DEFAULT_CONF_LOC.c_str());
    
    if( !getenv("DIGIDOCPP_OVERRIDE_CONF") && util::File::fileExists(USER_CONF_LOC))
        init(USER_CONF_LOC);
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
 * Gets default configuration file directory.
 * @return default configuration file directory full path.
 */
std::string digidoc::XmlConf::getDefaultConfDir()
{
    // the file path in conf is relative to the conf file's location
    const char *env = DEFAULT_CONF_LOC.c_str();
    if( env )
        return digidoc::util::File::directory( env );
    char *path = getcwd( NULL, 0 );
    std::string ret;
    if( path )
        ret = path;
    free( path );
    return ret;
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

        std::string conf_fullpath = getDefaultConfDir();
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
 * Sets Default (global) configuration file path in DEFAULT_CONF_LOC variable.
 */
void digidoc::XmlConf::setDefaultConfPath() throw(IOException)
{
    char * overrideConf = getenv( "DIGIDOCPP_OVERRIDE_CONF" );
    if (overrideConf != NULL) //if there is environment variable defined, use this conf instead of others
    {
        if(util::File::fileExists(overrideConf))
            DEFAULT_CONF_LOC = overrideConf;
        else
            THROW_IOEXCEPTION("Error loading override xml configuration from '%s' file",overrideConf);
    }
    
    else
    {
    #ifdef _WIN32
        HKEY hkey;
        DWORD dwSize;
        TCHAR tcConfPath[MAX_PATH];
        //Open Registry to get path for default configuration file
        if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT(DIGIDOCPP_PATH_REGISTRY_KEY), 0, KEY_QUERY_VALUE, &hkey)==ERROR_SUCCESS)
        {
            dwSize = MAX_PATH * sizeof(TCHAR);
            if (RegQueryValueEx(hkey, "ConfigFile", NULL, NULL, (LPBYTE)tcConfPath, &dwSize)==ERROR_SUCCESS)
                DEFAULT_CONF_LOC = tcConfPath;
            else //if couldn't open regkey value 
                THROW_IOEXCEPTION("Failed to open registry key \"%s\" ConfigFile value ", DIGIDOCPP_PATH_REGISTRY_KEY);
            RegCloseKey(hkey);
        }
        else //if key doesn't exist
            THROW_IOEXCEPTION("Failed to read registry key \"%s\"", DIGIDOCPP_PATH_REGISTRY_KEY);
    #else
        DEFAULT_CONF_LOC = DIGIDOCPP_CONFIG_DIR "/digidocpp.conf";
    #endif
    }
}

/**
 * Sets User specific configuration file path in USER_CONF_LOC variable.
 */
void digidoc::XmlConf::setUserConfPath()
{
#ifdef _WIN32
    USER_CONF_LOC = getenv ("APPDATA");
    if (USER_CONF_LOC.size())
        USER_CONF_LOC += "\\digidocpp\\digidocpp.conf";   
#else
    USER_CONF_LOC = getenv("HOME");
    if (USER_CONF_LOC.size())
        USER_CONF_LOC += "/.digidocpp/digidocpp.conf"; 
#endif
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

/**
 * Gets user specific configuration file directory.
 * @return returns user configuration file directory.
 */
std::string digidoc::XmlConf::getUserConfDir() const
{
    return util::File::directory(USER_CONF_LOC);
}

/**
 * Gets Manifest schema file location.
 * @return Manifest schema full path location.
 */
std::string digidoc::XmlConf::getManifestXsdPath() const
{
    return digidoc::util::File::fullPathUrl(getDefaultConfDir(), manifestXsdPath);
}

/**
 * Gets Xades schema file location.
 * @return Xades schema full path location.
 */
std::string digidoc::XmlConf::getXadesXsdPath() const
{
    return digidoc::util::File::fullPathUrl(getDefaultConfDir(), xadesXsdPath);
}

/**
 * Gets Dsig schema file location.
 * @return Dsig schema full path location.
 */
std::string digidoc::XmlConf::getDsigXsdPath() const
{
    return digidoc::util::File::fullPathUrl(getDefaultConfDir(), dsigXsdPath);
}

/**
 * Gets PKCS11 driver file path.
 * @return PKCS11 driver file location.
 */
std::string digidoc::XmlConf::getPKCS11DriverPath() const
{
    return pkcs11DriverPath;
}

/**
 * Gets OCSP data by issuer.
 * @param issuer OCSP issuer.
 * @return returns OCSP data structure, containing issuer, url and certificate location.
 */
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

/**
 * Gets Certificate store location.
 * @return Certificate store full path location.
 */
std::string digidoc::XmlConf::getCertStorePath() const
{
    return getDefaultConfDir() + "/" + certStorePath;
}

/**
 * Gets proxy host address.
 * @return proxy host address.
 */
std::string digidoc::XmlConf::getProxyHost() const
{
    return proxyHost;
}

/**
 * Gets proxy port number.
 * @return proxy port.
 */
std::string digidoc::XmlConf::getProxyPort() const
{
    return proxyPort;
}

/**
 * Gets proxy user name.
 * @return proxy user name.
 */
std::string digidoc::XmlConf::getProxyUser() const
{
    return proxyUser;
}

/**
 * Gets proxy login password.
 * @return proxy password.
 */
std::string digidoc::XmlConf::getProxyPass() const
{
    return proxyPass;
}

/**
 * Gets PKCS12 certificate file location.
 * @return PKCS12 certificate full path location.
 */
std::string digidoc::XmlConf::getPKCS12Cert() const
{
    return pkcs12Cert;
}

/**
 * Gets PKCS12 password.
 * @return PKCS12 password.
 */
std::string digidoc::XmlConf::getPKCS12Pass() const
{
    return pkcs12Pass;
}

/**
 * Sets a Proxy host address. Also adds or replaces proxy host data in the user configuration file.
 *
 * @param host proxy host address.
 * @throws IOException exception is thrown if saving a proxy host address into a user configuration file fails.
 */
void digidoc::XmlConf::setProxyHost( const std::string &host ) throw(IOException)
{
    proxyHost = host;
    setUserConf(PROXY_HOST, host);
}

/**
 * Sets a Proxy port number. Also adds or replaces proxy port data in the user configuration file.
 *
 * @param port proxy port number.
 * @throws IOException exception is thrown if saving a proxy port number into a user configuration file fails.
 */
void digidoc::XmlConf::setProxyPort( const std::string &port ) throw(IOException)
{
    proxyPort = port;
    setUserConf(PROXY_PORT, port);
}

/**
 * Sets a Proxy user name. Also adds or replaces proxy user name in the user configuration file.
 *
 * @param user proxy user name.
 * @throws IOException exception is thrown if saving a proxy user name into a user configuration file fails.
 */
void digidoc::XmlConf::setProxyUser( const std::string &user ) throw(IOException)
{
    proxyUser = user;
    setUserConf(PROXY_USER, user);
}

/**
 * Sets a Proxy password. Also adds or replaces proxy password in the user configuration file.
 *
 * @param pass proxy password.
 * @throws IOException exception is thrown if saving a proxy password into a user configuration file fails.
 */
void digidoc::XmlConf::setProxyPass( const std::string &pass ) throw(IOException)
{
    proxyPass = pass;
    setUserConf(PROXY_PASS, pass);
}

/**
 * Sets a PKCS#12 certficate path. Also adds or replaces PKCS#12 certificate path in the user configuration file.
 * By default the PKCS#12 certificate file should be located at default path, given by getUserConfDir() function.
 *
 * @param cert PKCS#12 certificate location path.
 * @throws IOException exception is thrown if saving a PKCS#12 certificate path into a user configuration file fails.
 */
void digidoc::XmlConf::setPKCS12Cert( const std::string &cert ) throw(IOException)
{
    pkcs12Cert = cert;
    setUserConf(PKCS12_CERT, cert);
}

/**
 * Sets a PKCS#12 certificate password. Also adds or replaces PKCS#12 certificate password in the user configuration file.
 *
 * @param pass PKCS#12 certificate password.
 * @throws IOException exception is thrown if saving a PKCS#12 certificate password into a user configuration file fails.
 */
void digidoc::XmlConf::setPKCS12Pass( const std::string &pass ) throw(IOException)
{
    pkcs12Pass = pass;
    setUserConf(PKCS12_PASS, pass);
}

/**
 * Adds or replaces OCSP parameters in the user configuration file.
 *
 * @param issuer an ocsp certificate issuer.
 * @param url an OCSP server url.
 * @param cert an OCSP certificate location path.
 * @throws IOException exception is thrown if saving an OCSP parameters into a user configuration file fails.
 */
void digidoc::XmlConf::setOCSP(const std::string &issuer, const std::string &url, const std::string &cert) throw(IOException)
{
    OCSPConf confData;
    confData.issuer = issuer;
    confData.url = url;
    confData.cert = cert;
    setUserOCSP(confData);
}

/**
 * Sets any parameter in a user configuration file. Also creates a configuration file if it is missing.
 *
 * @param paramName name of parameter that needs to be changed or created.
 * @param value value for changing or adding to a given parameter. If value is an empty string, the walue for a given parameter will be erased.
 * @throws IOException exception is thrown if reading, writing or creating of a user configuration file fails.
 */
void digidoc::XmlConf::setUserConf(const std::string &paramName, const std::string &value) throw(IOException)
{
    Param newParam(value, paramName);
    std::auto_ptr< ::Configuration > conf;
    Configuration::ParamSequence paramSeq;

    try
    {
        if(util::File::fileExists(USER_CONF_LOC))
        {
            
            //open user conf file
            conf = configuration (USER_CONF_LOC, xml_schema::Flags::dont_initialize);
            paramSeq = conf->param();
            Configuration::ParamSequence::iterator it;

            for( it = paramSeq.begin(); it != paramSeq.end(); it++)
            {
                if (paramName.compare(it->name()) == 0)
                {
                    paramSeq.erase(it);
                    if (value.size()) //if we do not want to just erase
                        paramSeq.insert(it, newParam);    
                    break;
                }
            }

            if (it == paramSeq.end() && value.size()) //if it's a new parameter
                paramSeq.push_back(newParam);
        }
        else
        {
            //Check if directory exists
            if (!util::File::directoryExists(getUserConfDir()))
                util::File::createDirectory(getUserConfDir());

            //create a new file
            //copy global conf and erase data
            conf = configuration (DEFAULT_CONF_LOC, xml_schema::Flags::dont_initialize);
            Configuration::OcspSequence ocspSeq;
            paramSeq.push_back(newParam);
            conf->ocsp(ocspSeq); //replace all ocsp data with empty ocsp sequence
        }
        conf->param(paramSeq); //replace all param data with new modified param sequence
    }
    catch (const xml_schema::Exception& e)
    {
        std::ostringstream oss;
        oss << e;
        THROW_IOEXCEPTION("(in set %s) Failed to parse configuration: %s", paramName.c_str(), oss.str().c_str());
    }
    serializeUserConf(*conf);
}

/**
 * Sets OCSP configuration parmeters in a user configuration file. Also creates a configuration file if it is missing.
 *
 * @param ocspData OCSP configuration structure defined in Conf.h (OCSPConf). Contains: OCSP issuer, OCSP URL and OCSP certificate location. Empty URL or cert location will erase this parameter for given issuer.
 * @throws IOException exception is thrown if reading, writing or creating of a user configuration file fails.
 */
void  digidoc::XmlConf::setUserOCSP(const Conf::OCSPConf &ocspData) throw(IOException)
{
    Ocsp newOcsp(ocspData.url, ocspData.cert, ocspData.issuer);
    std::auto_ptr< ::Configuration > conf;
    Configuration::OcspSequence ocspSeq;
    try
    {
        if(util::File::fileExists(USER_CONF_LOC))
        {
            conf = configuration (USER_CONF_LOC, xml_schema::Flags::dont_initialize);
            ocspSeq = conf->ocsp();
            Configuration::OcspSequence::iterator it;

            for(it = ocspSeq.begin(); it != ocspSeq.end(); ++it)
            {
                if (ocspData.issuer.compare(it->issuer()) == 0)
                {
                    ocspSeq.erase(it);
                    if (ocspData.url.size() || ocspData.cert.size()) //if we do not want to just erase
                       ocspSeq.insert(it, newOcsp);  
                    break;
                }
            }
            if (it == ocspSeq.end() && (ocspData.url.size() || ocspData.cert.size())) //if it's a new parameter
                ocspSeq.push_back(newOcsp);
        }
        else
        {
            //Check if directory exists
            if (!util::File::directoryExists(getUserConfDir()))
                util::File::createDirectory(getUserConfDir());
               
            //create a new file
            //copy global conf and erase data
            conf = configuration (DEFAULT_CONF_LOC, xml_schema::Flags::dont_initialize);
            Configuration::ParamSequence paramSeq;            
            conf->param(paramSeq); //replace all param data with empty param sequence
            ocspSeq.push_back(newOcsp);
        }        
        conf->ocsp(ocspSeq); //replace all ocsp data with new modified ocsp sequence
    }
    catch(const xml_schema::Exception& e)
    {
        std::ostringstream oss;
        oss << e;
        THROW_IOEXCEPTION("(in set OCSP) Failed to parse configuration: %s", oss.str().c_str());
    }
    serializeUserConf(*conf);
}

/**
 * Writes configuration data to user specific configuration xml file.
 *
 * @param pConf configuration data in xsd object using conf.xsd schema.
 * @throws IOException exception is thrown if opening of user configuration file fails.
 */
void digidoc::XmlConf::serializeUserConf(const ::Configuration &pConf) throw(IOException)
{
    std::ofstream ofs;
    ofs.open(USER_CONF_LOC.c_str());
    if (ofs.fail())
    {
        ofs.close(); //just in case it was left open
        THROW_IOEXCEPTION("Failed to open configuration: %s", USER_CONF_LOC.c_str());
    }
    xml_schema::NamespaceInfomap map;
    map[""].name = "";
    map[""].schema = getConfSchemaLocation().c_str();
    configuration(ofs, pConf, map);
    ofs.close();
}

/**
 * Gets configuration file schema location with full path.
 * @throws IOException exception is thrown if failed to parse schema location from default conf file.
 * @return returns configuration file schema location.
 */
std::string digidoc::XmlConf::getConfSchemaLocation() throw(IOException)
{
    xml_schema::ErrorHandler *errH = NULL;
    xml_schema::dom::auto_ptr< xercesc::DOMDocument > domDoc;
    try
    {
        //Parse default configuration file into a DOM document
        domDoc = ::xsd::cxx::xml::dom::parse< char > ( DEFAULT_CONF_LOC, *errH, 
            xml_schema::Properties (), xml_schema::Flags::dont_initialize );
    }
    catch (...)
    {
        THROW_IOEXCEPTION("Failed to parse %s into DOMDocument", DEFAULT_CONF_LOC.c_str());
    }

    //Get access to root element
    xercesc::DOMElement* root = domDoc->getDocumentElement();
    //Get schema location atributes.
    const XMLCh* xsl = root->getAttributeNS (
        xercesc::SchemaSymbols::fgURI_XSI,
        xercesc::SchemaSymbols::fgXSI_NONAMESPACESCHEMALOCACTION);
    
    if (xsl == NULL)
        THROW_IOEXCEPTION("Failed to parse schema location in: %s", DEFAULT_CONF_LOC.c_str());

    return digidoc::util::File::path ( getDefaultConfDir(), 
        ::xsd::cxx::xml::transcode<char> (xsl) );
}