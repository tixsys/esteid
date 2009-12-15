/*
 * XmlConf.cpp
 */

#include "XmlConf.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
//#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#include <Winreg.h>
#include <direct.h>
#endif

#include "log.h"
#include "util/File.h"
#include "util/String.h"
#include "xml/conf.hxx"

#include <stdlib.h>//getenv

#ifdef _WIN32
#define EST_ID_CARD_PATH "SOFTWARE\\Estonian ID Card\\digidocpp"
#define ENVIRONMENT_PATH "Volatile Environment"
#endif

/**
 * Environment variable name, that is used for loading configuration
 */
//const std::string digidoc::XmlConf::CONF_ENV = "BDOCLIB_CONF_XML"; //<-- no need anymore

/**
 * Path to default configuration file
 */
std::string digidoc::XmlConf::DEFAULT_CONF_LOC = "digidocpp.conf";
std::string digidoc::XmlConf::USER_CONF_LOC = "digidocpp.conf";

const std::string digidoc::XmlConf::CONF_NAMESPACE = "http://www.w3.org/2001/XMLSchema-instance";

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
	//Define Global Conf file location
#ifdef _WIN32
	HKEY hkey;
	DWORD dwSize;
	TCHAR tcConfPath1[1024];
	TCHAR tcConfPath2[1024];
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT(EST_ID_CARD_PATH), 0, KEY_QUERY_VALUE, &hkey)==ERROR_SUCCESS)
	{
		dwSize = 1024 * sizeof(TCHAR);
		RegQueryValueEx(hkey, TEXT("ConfigFile"), NULL, NULL, (LPBYTE)tcConfPath1, &dwSize);
		RegCloseKey(hkey);
		DEFAULT_CONF_LOC = tcConfPath1;	
	}	

	if(RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(ENVIRONMENT_PATH), 0, KEY_QUERY_VALUE, &hkey)==ERROR_SUCCESS)
	{
		dwSize = 1024 * sizeof(TCHAR);
		RegQueryValueEx(hkey, TEXT("APPDATA"), NULL, NULL, (LPBYTE)tcConfPath2, &dwSize);
		RegCloseKey(hkey);

		USER_CONF_LOC = tcConfPath2;
		USER_CONF_LOC += "\\digidocpp\\digidocpp.conf";

	//	DEFAULT_CONF_LOC = tcConfPath2;
	//	DEFAULT_CONF_LOC += "\\digidocpp\\digidocpp.conf";
	//	USER_CONF_LOC = "C:\\Program Files\\Estonian ID Card\\user_digidocpp.conf";
	}
#else
	DEFAULT_CONF_LOC = DIGIDOCPP_CONFIG_DIR "digidocpp.conf";
	
	USER_CONF_LOC = getenv("HOME");
	USER_CONF_LOC += "/.digidocpp/digidocpp.conf";
#endif

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
    std::string defaultConfLoc("digidocpp.conf");
//	getUserConfPath();
/*    char * envLoc = getenv (CONF_ENV.c_str());
    if(envLoc != NULL)
    {
        std::string envConfLoc(envLoc);

        if(util::File::fileExists(envConfLoc))
        {
            init(envConfLoc);			
			if(util::File::fileExists(USER_CONF_LOC))
				init(USER_CONF_LOC);
			return;
        }
    }
*/
	if(util::File::fileExists(DEFAULT_CONF_LOC))
    {
        init(DEFAULT_CONF_LOC);
		if(util::File::fileExists(USER_CONF_LOC))
			init(USER_CONF_LOC);
    }
    else
    {
  //      THROW_IOEXCEPTION("Error loading xml configuration from '%s' env variable or file '%s'",CONF_ENV.c_str(), DEFAULT_CONF_LOC.c_str());
	      THROW_IOEXCEPTION("Error loading xml configuration from '%s' file or '%s' file",DEFAULT_CONF_LOC.c_str(), USER_CONF_LOC.c_str());
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

std::string digidoc::XmlConf::fullpath() const
{
    // the file path in conf is relative to the conf file's location
    //const char *env = getenv( CONF_ENV.c_str() );
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

        std::string conf_fullpath = fullpath();
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

void digidoc::XmlConf::getUserConfDir() const
{
#ifdef _WIN32	

	HKEY hKey;
	DWORD dwSize;
	TCHAR ConfPath[1024];
	if(RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(ENVIRONMENT_PATH), 0, KEY_QUERY_VALUE, &hKey)==ERROR_SUCCESS)
	{
		dwSize = 1024 * sizeof(TCHAR);
		RegQueryValueEx(hKey, TEXT("APPDATA"), NULL, NULL, (LPBYTE)ConfPath, &dwSize);
		RegCloseKey(hKey);

		USER_CONF_LOC = ConfPath;
		USER_CONF_LOC += "\\digidocpp";
	}
#else
	USER_CONF_LOC = getenv("HOME");
	USER_CONF_LOC += "/.digidocpp";
#endif
}

std::string digidoc::XmlConf::getManifestXsdPath() const
{
    return digidoc::util::File::fullPathUrl(fullpath(), manifestXsdPath);
}

std::string digidoc::XmlConf::getXadesXsdPath() const
{
    return digidoc::util::File::fullPathUrl(fullpath(), xadesXsdPath);
}

std::string digidoc::XmlConf::getDsigXsdPath() const
{
    return digidoc::util::File::fullPathUrl(fullpath(), dsigXsdPath);
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
    return fullpath() + "/" + certStorePath;
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

void digidoc::XmlConf::setProxyHost( const std::string &host ) throw(IOException)
{
    proxyHost = host;
	try
	{
		setUserConf(PROXY_HOST, host);
	}
	catch((const IOException& e))
	{
		std::ostringstream oss;
		oss << e;
		THROW_IOEXCEPTION("Failed to set Proxy host: %s", oss.str().c_str());
	}

}

void digidoc::XmlConf::setProxyPort( const std::string &port ) throw(IOException)
{
    proxyPort = port;
	setUserConf(PROXY_PORT, port);
}

void digidoc::XmlConf::setProxyUser( const std::string &user ) throw(IOException)
{
    proxyUser = user;
	try
	{
		setUserConf(PROXY_USER, user);
	}
	catch((const IOException& e))
	{
		std::ostringstream oss;
		oss << e;
		THROW_IOEXCEPTION("Failed to set Proxy user: %s", oss.str().c_str());
	}
}

void digidoc::XmlConf::setProxyPass( const std::string &pass ) throw(IOException)
{
    proxyPass = pass;
	try
	{
		setUserConf(PROXY_PASS, pass);
	}
	catch((const IOException& e))
	{
		std::ostringstream oss;
		oss << e;
		THROW_IOEXCEPTION("Failed to set Proxy password: %s", oss.str().c_str());
	}
}

void digidoc::XmlConf::setPKCS12Cert( const std::string &cert ) throw(IOException)
{
    pkcs12Cert = cert;
	try
	{
		setUserConf(PKCS12_CERT, cert);
	}
	catch((const IOException& e))
	{
		std::ostringstream oss;
		oss << e;
		THROW_IOEXCEPTION("Failed to set PKCS12 cert: %s", oss.str().c_str());
	}
}

void digidoc::XmlConf::setPKCS12Pass( const std::string &pass ) throw(IOException)
{
    pkcs12Pass = pass;
	try
	{
		setUserConf(PKCS12_PASS, pass);
	}
	catch((const IOException& e))
	{
		std::ostringstream oss;
		oss << e;
		THROW_IOEXCEPTION("Failed to set PKCS12 password: %s", oss.str().c_str());
	}
}

void digidoc::XmlConf::setUserConf(const std::string &paramName, const std::string &value) throw(IOException)
{
	std::ofstream ofs;
	bool done = false;
	Param newParam(value, paramName);
	std::string confXsd = fullpath();
#ifdef _WIN32
	 for (int c=0; c<sizeof(confXsd); c++)
	 {
		 if (confXsd[c] == '\\')
			 confXsd[c] = '/';
	 }
#endif
	confXsd += "/schema/conf.xsd";

	if(util::File::fileExists(USER_CONF_LOC))
    {
		try
		{
			//open user conf file
			std::auto_ptr< ::Configuration > conf( configuration (USER_CONF_LOC, xml_schema::Flags::dont_initialize));
			Configuration::ParamSequence paramSeq = conf->param();

			for( Configuration::ParamSequence::iterator it = paramSeq.begin(); it != paramSeq.end(); it++)
			{
				if (paramName.compare(it->name()) == 0)
				{
					paramSeq.erase(it);
					break;
				}
			}
			if (value.size()) //if we do not want to just erase
				paramSeq.push_back(newParam);
			conf->param(paramSeq); //replace all param data with new modified param sequence

			ofs.open(USER_CONF_LOC.c_str());
			xml_schema::NamespaceInfomap map;
			map[""].name = "";
			map[""].schema = confXsd.c_str();
			configuration(ofs, *conf, map);	
		}
        catch(const xml_schema::Exception& e)
		{
			std::ostringstream oss;
			oss << e;
			THROW_IOEXCEPTION("Failed to parse configuration: %s", oss.str().c_str());
		}
	}	
	else
	{
		//create a new file
		//copy global conf and erase data
		std::auto_ptr< ::Configuration > conf( configuration (DEFAULT_CONF_LOC, xml_schema::Flags::dont_initialize));
		Configuration::OcspSequence ocspSeq;
		Configuration::ParamSequence paramSeq;
		paramSeq.push_back(newParam);
		conf->param(paramSeq); //replace all param data with new modified param sequence
		conf->ocsp(ocspSeq); //replace all ocsp data with empty ocsp sequence

		ofs.open(USER_CONF_LOC.c_str());
		//ofs.open("C:\\PERSE.XML");
		xml_schema::NamespaceInfomap map;
		map[""].name = "";
		map[""].schema = confXsd.c_str();
		configuration(ofs, *conf, map);

		/*
		ofs.open(USER_CONF_LOC.c_str());
		Configuration::ParamSequence paramSeq;
		paramSeq.push_back(newParam);
	
		Configuration conf;
		conf.param(paramSeq); //replace all param data with new modified param sequence

		ofs.open("C:\\PERSE.XML");
		xml_schema::NamespaceInfomap map;
		map[""].name = "";
		map[""].schema = confXsd.c_str();
		configuration(ofs, *conf, map);
		*/		
	}
	
	if (ofs.fail())
	{
		ofs.close(); //just in case it was left open
        THROW_IOEXCEPTION("Failed to open configuration: %s", USER_CONF_LOC.c_str());
	}
	ofs.close();
}

void  digidoc::XmlConf::setUserOCSP(const Conf::OCSPConf &ocspData) throw(IOException)
{
	std::ofstream ofs;
	bool done = false;
	Ocsp newOcsp(ocspData.url, ocspData.cert, ocspData.issuer);
	std::string confXsd = fullpath();
#ifdef _WIN32
	 for (int c=0; c<sizeof(confXsd); c++)
	 {
		 if (confXsd[c] == '\\')
			 confXsd[c] = '/';
	 }
#endif
	confXsd += "/schema/conf.xsd";

	if(util::File::fileExists(USER_CONF_LOC))
	{
		try
		{
			std::auto_ptr< ::Configuration > conf( configuration (USER_CONF_LOC, xml_schema::Flags::dont_initialize));
			Configuration::OcspSequence ocspSeq = conf->ocsp();
			for( Configuration::OcspSequence::iterator it = ocspSeq.begin(); it != ocspSeq.end(); ++it)
			{
				if (ocspData.issuer.compare(it->issuer()) == 0)
				{
					ocspSeq.erase(it);
					break;
				}
				if (ocspData.url.size() || ocspData.cert.size()) //if we do not want to just erase
					ocspSeq.push_back(newOcsp);
				conf->ocsp(ocspSeq); //replace all ocsp data with new modified ocsp sequence

				ofs.open(USER_CONF_LOC.c_str());
				xml_schema::NamespaceInfomap map;
				map[""].name = "";
				map[""].schema = confXsd.c_str();
				configuration(ofs, *conf, map);		
			}
		}
		catch(const xml_schema::Exception& e)
		{
			std::ostringstream oss;
			oss << e;
			THROW_IOEXCEPTION("Failed to parse configuration: %s", oss.str().c_str());
		}
	}
	else
	{
		//create a new file
		//copy global conf and erase data
		std::auto_ptr< ::Configuration > conf( configuration (DEFAULT_CONF_LOC, xml_schema::Flags::dont_initialize));
		Configuration::OcspSequence ocspSeq;
		Configuration::ParamSequence paramSeq;
		ocspSeq.push_back(newOcsp);
		conf->param(paramSeq); //replace all param data with empty param sequence
		conf->ocsp(ocspSeq); //replace all ocsp data with new modified ocsp sequence

		ofs.open(USER_CONF_LOC.c_str());
		xml_schema::NamespaceInfomap map;
		map[""].name = "";
		map[""].schema = confXsd.c_str();
		configuration(ofs, *conf, map);
	}
	if (ofs.fail())
	{
		ofs.close(); //just in case it was left open
        THROW_IOEXCEPTION("Failed to open configuration: %s", USER_CONF_LOC.c_str());
	}
	ofs.close();
}
