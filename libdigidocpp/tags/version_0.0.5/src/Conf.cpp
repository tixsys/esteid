#include "Conf.h"
#include "log.h"


//std::auto_ptr<digidoc::Conf> digidoc::Conf::INSTANCE = std::auto_ptr<digidoc::Conf>(new digidoc::Conf());
digidoc::Conf* digidoc::Conf::INSTANCE = NULL;

/**
 *
 * @return
 */
digidoc::Conf* digidoc::Conf::getInstance()
{
    //return INSTANCE.get();
    if(INSTANCE == NULL)
    {
        ERR("Conf not initialized");
    }
    return INSTANCE;
}

/**
 * Init global Conf with conf
 * @param conf implementation to use
 */
void digidoc::Conf::init(Conf* conf)
{
    //INSTANCE = std::auto_ptr<Conf>(conf);
    INSTANCE = conf;
}

digidoc::Conf::Conf()
{
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
std::string digidoc::Conf::getDigestUri() const
{
    return "http://www.w3.org/2000/09/xmldsig#sha1";
}

/**
 *
 * @return
 */
std::string digidoc::Conf::getManifestXsdPath() const
{
    // TODO: implement
    // TODO:NOTE:CAREFUL!!! CodeSynt goes crazy on windows when xsd path contains spaces. Currently,
    // the only way found that works is using this notation below:
    // "file:///C:/dev/Smartlink/Digidoc%20Cpp%20Teek/head/etc/schema/OpenDocument_manifest.xsd";
    return "file:///home/janno/projects/workspace/DigiDocLIB/etc/schema/OpenDocument_manifest.xsd";
    //return "file:///C:/dev/Smartlink/Digidoc%20Cpp%20Teek/head/etc/schema/OpenDocument_manifest.xsd";
}

/**
*
* @return
*/
std::string digidoc::Conf::getXadesXsdPath() const
{
    // TODO: implement
    // TODO:NOTE:CAREFUL!!! See getManifestXsdPath()
    //return "file:///C:/dev/Smartlink/Digidoc%20Cpp%20Teek/head/etc/schema/XAdES.xsd";
    return "file:///home/janno/projects/workspace/DigiDocLIB/etc/schema/XAdES.xsd";
}

/**
*
* @return
*/
std::string digidoc::Conf::getDsigXsdPath() const
{
    // TODO: implement
    // TODO:NOTE:CAREFUL!!! See getManifestXsdPath()
    return "file:///home/janno/projects/workspace/DigiDocLIB/etc/schema/xmldsig-core-schema.xsd";
}

/**
 *
 * @return
 */
std::string digidoc::Conf::getPKCS11DriverPath() const
{
    return "/usr/lib/opensc-pkcs11.so";
}

/**
 *
 * @return
 */
std::string digidoc::Conf::getOCSPUrl() const
{
    return "http://www.openxades.org/cgi-bin/ocsp.cgi";
}

/**
 *
 * @return path to single PEM file that contains OCSP responder certificate(s).
 */
std::string digidoc::Conf::getOCSPCertPath() const
{
    return "/home/janno/projects/workspace/DigiDocLIB/etc/certs/sk-test-ocsp-responder-2005.pem";
}

/**
 *
 * @return
 */
std::string digidoc::Conf::getCertStorePath() const
{
    return "/home/janno/projects/workspace/DigiDocLIB/etc/certs";
}
