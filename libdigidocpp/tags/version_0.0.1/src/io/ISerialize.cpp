#include "ISerialize.h"


/**
 * Initializes serializer with path.
 *
 * @param path
 */
digidoc::ISerialize::ISerialize(const std::string& path)
 : path(path)
{
}

/**
 * @return returns serializer path.
 */
std::string digidoc::ISerialize::getPath()
{
    return path;
}
