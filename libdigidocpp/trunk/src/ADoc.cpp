#include "ADoc.h"

#include "XmlConf.h"
#include "util/File.h"

#include <xsec/utils/XSECPlatformUtils.hpp>

/**
 * Initializes libraries used in digidoc implementation.
 */
void digidoc::initialize()
{
    // Initialize OpenSSL library.
    SSL_load_error_strings();
    SSL_library_init();
    OPENSSL_config(NULL);

    // Initialize Apache Xerces library.
    xercesc::XMLPlatformUtils::Initialize();

    // Initialize Apache XML Security library.
    XSECPlatformUtils::Initialise();

    //Use Xml based configuration
    XmlConf::initialize();
}

/**
 * Terminates libraries used in digidoc implementation.
 */
void digidoc::terminate()
{
    // Terminate Apache XML Security library.
    XSECPlatformUtils::Terminate();

    // Terminate Apache Xerces library.
    xercesc::XMLPlatformUtils::Terminate();

    // Try to delete temporary files created with the util::File::tempFileName, tempDirectory and createTempDirectory functions.
    util::File::deleteTempFiles();
}
