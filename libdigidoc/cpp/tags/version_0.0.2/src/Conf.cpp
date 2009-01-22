#include "Conf.h"


std::auto_ptr<digidoc::Conf> digidoc::Conf::INSTANCE = std::auto_ptr<digidoc::Conf>(new digidoc::Conf());

/**
 *
 * @return
 */
digidoc::Conf* digidoc::Conf::getInstance()
{
    // TODO: implement
    return INSTANCE.get();
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
    return "file:///C:/dev/Smartlink/Digidoc%20Cpp%20Teek/head/etc/schema/OpenDocument_manifest.xsd";
}

/**
*
* @return
*/
std::string digidoc::Conf::getXadesXsdPath() const
{
    // TODO: implement
    // TODO:NOTE:CAREFUL!!! See getManifestXsdPath()
    return "file:///C:/dev/Smartlink/Digidoc%20Cpp%20Teek/head/etc/schema/XAdES.xsd";
}

/**
*
* @return
*/
std::string digidoc::Conf::getDsigXsdPath() const
{
    // TODO: implement
    // TODO:NOTE:CAREFUL!!! See getManifestXsdPath()
    return "file:///C:/dev/Smartlink/Digidoc%20Cpp%20Teek/head/etc/schema/xmldsig-core-schema.xsd";
}
