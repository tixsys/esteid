#if defined(_WIN32)
#include <conio.h>
#endif

#include <string.h>

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
 * EstEID ID-Card signer with interactive acquisition from console.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @param prompt prompt that is printed to console if PIN code is acquired. The prompt
 *        must cotain <code>%s</code>, it is replaced with token label. For example
 *        with <pre>ID-kaart (PIN2, Allkirjastamine)</pre>.
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDConsolePinSigner::EstEIDConsolePinSigner(const std::string& driver, const std::string& prompt) throw(SignException)
 : EstEIDSigner(driver)
 , prompt(prompt)
{
}

/**
 * EstEID ID-Card signer with interactive acquisition from console.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDConsolePinSigner::EstEIDConsolePinSigner(const std::string& driver) throw(SignException)
 : EstEIDSigner(driver)
 , prompt("Please enter PIN for token '%s' or <enter> to cancel: ")
{
}

/**
 *
 */
digidoc::EstEIDConsolePinSigner::~EstEIDConsolePinSigner()
{
}

/**
 * @param prompt sets the prompt value.
 */
void digidoc::EstEIDConsolePinSigner::setPrompt(const std::string& prompt)
{
    this->prompt = prompt;
}

/**
 * Asks PIN code from the user using console and returns it.
 *
 * @param certificate the certificate, which PIN code is asked.
 * @return returns PIN code.
 * @throws SignException throws an exception if the login process is canceled.
 */
std::string digidoc::EstEIDConsolePinSigner::getPin(PKCS11Cert certificate) throw(SignException)
{
    char pin[16];
    size_t pinMax = 16;

#if defined(_WIN32)
    {
        printf(prompt.c_str(), certificate.token.label.c_str());
        size_t i = 0;
        char c;
        while(i < pinMax && (c = getch()) != '\r')
        {
            pin[i++] = c;
        }
    }
#else
    char* pwd = getpass(util::String::format(prompt.c_str(), certificate.token.label.c_str()).c_str());
    strncpy(pin, pwd, pinMax);
#endif

    pin[pinMax-1] = '\0';

    std::string result(pin);
    if(result.empty())
    {
        THROW_SIGNEXCEPTION("PIN acquisition canceled.");
    }

    return result;
}
