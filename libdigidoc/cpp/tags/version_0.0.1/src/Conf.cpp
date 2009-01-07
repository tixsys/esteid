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
	return "/home/janari/projects/smartlink/digidoc/etc/OpenDocument_manifest.xsd";
}
