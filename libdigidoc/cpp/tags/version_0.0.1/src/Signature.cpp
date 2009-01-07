#include "Signature.h"


/**
 *
 */
digidoc::Signature::Signature(const std::string& path, const std::string& mediaType)
{
}

/**
 * @return returns signature mimetype.
 */
std::string digidoc::Signature::getMediaType() const
{
	// TODO: implement
	return "signature/bdoc-1.0/TS";
}

/**
 * Saves signature to file using XAdES XML format.
 *
 * @return returns path of the signature XML.
 * @throws IOException throws exception if the signature file creation failed.
 */
std::string digidoc::Signature::saveToXml() const throw(IOException)
{
	// TODO: implement
	// Kui tegemist on ZIP conteinerist üles loetud allkirjaga, siis võiks
	// see meetod tagastada selle xml faili asukoha, kus ta üles loeti.
	// Ei pea uut xml koostama.
    return "/proc/cpuinfo";
}

