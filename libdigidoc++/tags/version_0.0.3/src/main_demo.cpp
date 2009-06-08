#if defined(_WIN32)
#include <conio.h>
#endif

#include "log.h"
#include "BDoc.h"
#include "crypto/cert/DirectoryX509CertStore.h"
#include "crypto/signer/EstEIDSigner.h"
#include "io/ZipSerialize.h"


using namespace digidoc;

const char* CERT_STATUSES[3] = { "GOOD", "REVOKED", "UNKNOWN" };

void printUsage(const char* executable);
int list(int argc, char* argv[]);
int create(int argc, char* argv[]);

/**
 * Implements PIN acquisition.
 */
class EstEIDConsolePinSigner : public EstEIDSigner
{

  public:
      EstEIDConsolePinSigner(const std::string& driver) throw(SignException);
      virtual ~EstEIDConsolePinSigner();

  protected:
      virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException);
      virtual std::string getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException);

  private:
      void printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert);

};



/**
 * Executes digidoc demonstration application.
 *
 * @param argc number of command line arguments.
 * @param argv command line arguments.
 * @return 0 - failure, 1 - success
 */
int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printUsage(argv[0]);
        return 0;
    }

    // Execute digidoc demonstration.
    if(std::string("list") == std::string(argv[1]))
    {
        return list(argc, argv);
    }
    else if(std::string("create") == std::string(argv[1]))
    {
        return create(argc, argv);
    }

    printUsage(argv[0]);
    return 0;
}

/**
 * Prints demo application usage.
 */
void printUsage(const char* executable)
{
    printf("Usage: %s list|create OPTIONS FILE\n\n", executable);
    printf("  Command list:\n");
    printf("    Example: %s list --validateOnline container-file.bdoc\n", executable);
    printf("    Available options:\n");
    printf("      --validateOnline  - optional, default false. Whether online validate all signatures\n");
    printf("  Command create:\n");
    printf("    Example: %s create --file=file1.txt --file=file2.txt --profile=TM demo-container.bdoc\n", executable);
    printf("    Available options:\n");
    printf("      --file    - optional, default is this demo application. The option can occur multiple times. File(s) to be signed\n");
    printf("      --profile - optional, default BES. Signing profile, available values: BES, TM\n");
}

/**
 * List documents and signatures inside BDOC container.
 * Example command: <code>demo list --validateOnline container-file.bdoc</code>
 * Args:
 * <ul>
 *   <li><b>--validateOnline</b> - optional, default false. Whether online validate all signatures</li>
 * </ul>
 *
 * @param argc number of command line arguments.
 * @param argv command line arguments.
 * @return 0 - failure, 1 - success
 */
int list(int argc, char* argv[])
{
    bool validateOnline = false;
    std::string path = "";

    // Parse command line arguments.
    for(int i = 2; i < argc; i++)
    {
        std::string arg(argv[i]);
        if(std::string("--validateOnline") == arg)
        {
            validateOnline = true;
        }
        else
        {
            path = arg;
        }
    }

    // No bdoc container path given.
    if(path.empty())
    {
        printUsage(argv[0]);
        return 0;
    }

    // Initialize digidoc library.
    initialize();


    try
    {
        // Init certificate store.
        X509CertStore::init(new DirectoryX509CertStore());

        // Open BDOC container.
        std::auto_ptr<ISerialize> serializer(new ZipSerialize(path));
        BDoc bdoc(serializer);

        // Print container document list.
        printf("Documents:\n");
        for(unsigned int i = 0; i < bdoc.documentCount(); i++)
        {
            Document doc = bdoc.getDocument(i);
            printf("  Document %u (%s): %s (%lu bytes)\n", i, doc.getMediaType().c_str(), doc.getPath().c_str(), doc.getSize());
        }

        // Print container signatures list.
        printf("\nSignatures:\n");
        for(unsigned int i = 0; i < bdoc.signatureCount(); i++)
        {
            // Get signature from bdoc cotainer.
            const Signature* sig = bdoc.getSignature(i);

            printf("  Signature %u (%s):\n", i, sig->getMediaType().c_str());

            // Validate signature offine. Checks, whether signature format is correct
            // and signed documents checksums are correct.
            try
            {
                sig->validateOffline();
                printf("    Offline validation: OK\n");
            }
            catch(const SignatureException& e)
            {
                // If signature is not offline valid, other data fields can not be read from it.
                printf("    Offline validation: FAILED\n");
                continue;
            }

            // Validate signature online. Checks the validity of the certificate used to sign the bdoc container.
            try
            {
                printf("    Online validation: %s\n", CERT_STATUSES[sig->validateOnline()]);
            }
            catch(const SignatureException& e)
            {
                printf("    Online validation: FAILED (%s)\n", e.getMsg().c_str());
            }

            // Get signature production place info.
            Signer::SignatureProductionPlace spp = sig->getProductionPlace();
            if(!spp.isEmpty())
            {
                printf("    Signature production place:\n");
                printf("      Country:           %s\n", spp.countryName.c_str());
                printf("      State or Province: %s\n", spp.stateOrProvince.c_str());
                printf("      City:              %s\n", spp.city.c_str());
                printf("      Postal code:       %s\n", spp.postalCode.c_str());
            }

            // Get signer role info.
            Signer::SignerRole roles = sig->getSignerRole();
            if(!roles.isEmpty())
            {
                printf("    Signer role(s):\n");
                for(std::vector<std::string>::const_iterator iter = roles.claimedRoles.begin(); iter != roles.claimedRoles.end(); iter++)
                {
                    printf("      %s\n", iter->c_str());
                }
            }

            // Get signing time.
            printf("    Signing time: %s\n", sig->getSigningTime().c_str());

            // Get signer certificate.
            // TODO: method getSigningCertificate() does not work, implement cert printing after it is fixed.
            //X509Cert cert = sig->getSigningCertificate();
        }

        // Destroy certificate store.
        X509CertStore::destroy();
    }
    catch(const digidoc::BDocException& e)
    {
        printf("Caught BDocException: %s\n", e.getMsg().c_str());
    }
    catch(const digidoc::IOException& e)
    {
        printf("Caught IOException: %s\n", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
    {
        printf("Caught Exception: %s\n", e.getMsg().c_str());
    }
    catch(...)
    {
        printf("Caught unknown exception\n");
    }

    // Terminate digidoc library.
    terminate();

    return 1;
}

/**
 * Create new BDOC container container.
 * Example command: <code>demo create --file=file1.txt --file=file2.txt --profile=TM demo-container.bdoc</code>
 * Args:
 * <ul>
 *   <li><b>--file</b> - optional, default is this demo application. The option can occur multiple times. File(s) to be signed</li>
 *   <li><b>--profile</b> - optional, default BES. Signing profile, available values: BES, TM</li>
 * </ul>
 *
 * @param argc number of command line arguments.
 * @param argv command line arguments.
 * @return 0 - failure, 1 - success
 */
int create(int argc, char* argv[])
{
    std::vector<std::string> files;
    Signature::Type profile = Signature::BES;
    std::string path = "";

    // Parse command line arguments.
    for(int i = 2; i < argc; i++)
    {
        std::string arg(argv[i]);

        if(arg.find("--file=") == 0)
        {
            files.push_back(arg.substr(7));
        }
        else if(arg.find("--profile=") == 0)
        {
            arg = arg.substr(10);
            if(std::string("BES") == arg)     { profile = Signature::BES; }
            else if(std::string("TM") == arg) { profile = Signature::TM;  }
            else
            {
                printUsage(argv[0]);
                return 0;
            }
        }
        else
        {
            path = argv[i];
        }
    }

    // No bdoc container path given.
    if(path.empty())
    {
        printUsage(argv[0]);
        return 0;
    }

    // If no files given to sign, sign the demo executable.
    if(files.empty())
    {
        files.push_back(argv[0]);
    }

    // Initialize digidoc library.
    initialize();


    try
    {
        // Init certificate store.
        X509CertStore::init(new DirectoryX509CertStore());

        // Create new BDOC container.
        BDoc bdoc;

        // Add all the documents to the container.
        for(std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
        {
            bdoc.addDocument(Document(*iter, "file"));
        }

        // Initialize signer implementation.
        EstEIDConsolePinSigner signer("/usr/lib/opensc-pkcs11.so");

        // Add signature production place.
        Signer::SignatureProductionPlace spp("Tallinn", "Harjumaa", "12345", "Estonia");
        signer.setSignatureProductionPlace(spp);

        // Add signer role(s).
        Signer::SignerRole role("CEO");
        // Add additional roles.
        role.claimedRoles.push_back("Taxi driver");
        signer.setSignerRole(role);

        // Sign the BDOC container.
        bdoc.sign(&signer, profile);

        // Save the BDOC container.
        std::auto_ptr<ISerialize> serializer(new ZipSerialize(path));
        bdoc.saveTo(serializer);

        // Destroy certificate store.
        X509CertStore::destroy();
    }
    catch(const digidoc::BDocException& e)
    {
        ERR("Caught BDocException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::IOException& e)
    {
        ERR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::SignException& e)
    {
        ERR("Caught SignException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
    {
        ERR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERR("Caught unknown exception");
    }

    // Terminate digidoc library.
    terminate();

    return 1;
}

/**
 * Console PIN acquisition implementation.
 */
EstEIDConsolePinSigner::EstEIDConsolePinSigner(const std::string& driver) throw(SignException)
 : EstEIDSigner(driver)
{
}

EstEIDConsolePinSigner::~EstEIDConsolePinSigner()
{
}

/**
 * Just show available signing certificates. Uses base class EstEIDSigner implementation
 * to choose the correct signing certificate.
 *
 * NB! If you wan to choose the certificate yourself use PKCS11Signer as base class
 * and implement the <code>selectSigningCertificate()</code> method.
 */
PKCS11Signer::PKCS11Cert EstEIDConsolePinSigner::selectSigningCertificate(
		std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException)
{
    // Print available certificates.
    printf("Available certificates:\n");
    for(std::vector<PKCS11Signer::PKCS11Cert>::const_iterator iter = certificates.begin(); iter != certificates.end(); iter++)
    {
        printPKCS11Cert(*iter);
    }

    PKCS11Signer::PKCS11Cert cert = EstEIDSigner::selectSigningCertificate(certificates);
    printf("Selected certificate: %s\n", cert.token.label.c_str());
    return cert;
}

/**
 * Ask PIN from the user and return it. If you want to cancel the signing
 * process throw an exception.
 *
 * @param certificate the certificate, which PIN code is asked.
 * @return should return PIN code.
 * @throws SignException should throw an exception to cancel login process.
 */
std::string EstEIDConsolePinSigner::getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException)
{
    char prompt[1024];
    char pin[16];
    size_t pinMax = 16;

    snprintf(prompt, sizeof(prompt), "Please enter PIN for token '%s' or <enter> to cancel: ", certificate.token.label.c_str());

#if defined(_WIN32)
    {
        size_t i = 0;
        char c;
        while(i < pinMax && (c = getch()) != '\r')
        {
            pin[i++] = c;
        }
    }
#else
    char* p = NULL;
    p = getpass(prompt);
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
void EstEIDConsolePinSigner::printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert)
{
    printf("  -------------------------------------------------------\n");
    printf("  token label:        %s\n", cert.token.label.c_str());
    printf("  token manufacturer: %s\n", cert.token.manufacturer.c_str());
    printf("  token model:        %s\n", cert.token.model.c_str());
    printf("  token serial Nr:    %s\n", cert.token.serialNr.c_str());
    printf("  label: %s\n", cert.label.c_str());
}
