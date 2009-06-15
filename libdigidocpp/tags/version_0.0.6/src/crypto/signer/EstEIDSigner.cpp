#if defined(_WIN32)
#include <conio.h>
#endif
#include "../../log.h"
#include "../../util/String.h"
#include "EstEIDSigner.h"


/**
 * Initializes base class PKCS11Signer.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDSigner::EstEIDSigner(const std::string& driver) throw(SignException)
 : PKCS11Signer(driver)
{
}

/**
 *
 */
digidoc::EstEIDSigner::~EstEIDSigner()
{
}

/**
 * Implements signing certificate selector for EstEID ID-Cards. Select signing
 * certificate, which has label 'Allkirjastamine'.
 *
 * @param certificates list of certificates to choose from. List of all certificates
 *        found ID-card.
 * @return should return the selected certificate.
 * @throws SignException throws exception if no suitable certificate was found.
 */
digidoc::PKCS11Signer::PKCS11Cert digidoc::EstEIDSigner::selectSigningCertificate(
        std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException)
{
    // Find EstEID signing certificate (PKCS11 cert label is string 'Allkirjastamine').
    for(std::vector<PKCS11Signer::PKCS11Cert>::const_iterator iter = certificates.begin(); iter != certificates.end(); iter++)
    {
        if(iter->label == std::string("Allkirjastamine"))
        {
            return *iter;
        }
    }

    THROW_SIGNEXCEPTION("Could not find certificate with label 'Allkirjastamine'.");
}

/**
 * EstEID ID-Card signer with interactive acquisition from console
 */
digidoc::EstEIDConsolePinSigner::EstEIDConsolePinSigner(const std::string& driver) throw(SignException)
 : EstEIDSigner(driver)
{
}

digidoc::EstEIDConsolePinSigner::~EstEIDConsolePinSigner()
{
}


/**
 * Just show available signing certificates. Uses base class EstEIDSigner implementation
 * to choose the correct signing certificate.
 *
 * NB! If you wan to choose the certificate yourself use PKCS11Signer as base class
 * and implement the <code>selectSigningCertificate()</code> method.
 */
//digidoc::PKCS11Signer::PKCS11Cert EstEIDConsolePinSigner::selectSigningCertificate(
//        std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException)
//{
//    // Print available certificates.
//    printf("Available certificates:\n");
//    for(std::vector<PKCS11Signer::PKCS11Cert>::const_iterator iter = certificates.begin(); iter != certificates.end(); iter++)
//    {
//        printPKCS11Cert(*iter);
//    }
//
//    PKCS11Signer::PKCS11Cert cert = EstEIDSigner::selectSigningCertificate(certificates);
//    printf("Selected certificate: %s\n", cert.token.label.c_str());
//    return cert;
//}

/**
 * Ask PIN from the user and return it. If you want to cancel the signing
 * process throw an exception.
 *
 * @param certificate the certificate, which PIN code is asked.
 * @return should return PIN code.
 * @throws SignException should throw an exception to cancel login process.
 */
std::string digidoc::EstEIDConsolePinSigner::getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException)
{
    char pin[16];
    size_t pinMax = 16;

#if defined(_WIN32)
    {
        printf("Please enter PIN for token '%s' or <enter> to cancel: ", certificate.token.label.c_str());
        size_t i = 0;
        char c;
        while(i < pinMax && (c = getch()) != '\r')
        {
            pin[i++] = c;
        }
    }
#else
    char prompt[1024];
    snprintf(prompt, sizeof(prompt), "Please enter PIN for token '%s' or <enter> to cancel: ", certificate.token.label.c_str());
    char* p = getpass(prompt);
    strncpy(pin, p, pinMax);
#endif

    pin[pinMax-1] = '\0';

    std::string result(pin);
    if(result.empty())
    {
        THROW_SIGNEXCEPTION("PIN acquisition canceled.");
    }

    return result;
}

/**
 * Prints certificate information.
 *
 * @param cert X.509 certificate.
 */
void digidoc::EstEIDConsolePinSigner::printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert)
{
    printf("  -------------------------------------------------------\n");
    printf("  token label:        %s\n", cert.token.label.c_str());
    printf("  token manufacturer: %s\n", cert.token.manufacturer.c_str());
    printf("  token model:        %s\n", cert.token.model.c_str());
    printf("  token serial Nr:    %s\n", cert.token.serialNr.c_str());
    printf("  label: %s\n", cert.label.c_str());
}

