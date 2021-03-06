/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "log.h"
#include "BDoc.h"
#include "Conf.h"
#include "Document.h"
#include "crypto/cert/DirectoryX509CertStore.h"
#include "crypto/signer/EstEIDSigner.h"
#include "io/ZipSerialize.h"


using namespace digidoc;

const char* CERT_STATUSES[3] = { "GOOD", "REVOKED", "UNKNOWN" };

void printUsage(const char* executable);
int list(int argc, char* argv[]);
int create(int argc, char* argv[]);

/**
 * For demonstration purpose overwrites certificate selection to print out all
 * the certificates available on ID-Card.
 */
class DemoEstEIDConsolePinSigner : public EstEIDConsolePinSigner
{

  public:
      DemoEstEIDConsolePinSigner(const std::string& driver) throw(SignException);
      virtual ~DemoEstEIDConsolePinSigner();

  protected:
      virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException);

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
    printf("      --pkcs11Driver - optional, default is '/usr/lib/opensc-pkcs11.so'. Path of PKCS11 driver.\n");
    printf("      --file         - optional, default is this demo application. The option can occur multiple times. File(s) to be signed\n");
    printf("      --profile      - optional, default BES. Signing profile, available values: BES, TM\n");
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
    digidoc::initialize();


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
                printf("    Offline validation: FAILED (%s)\n", e.getMsg().c_str());
                continue;
            }

            // Validate signature online. Checks the validity of the certificate used to sign the bdoc container.
            if(validateOnline)
            {
                try
                {
                    printf("    Online validation: %s\n", CERT_STATUSES[sig->validateOnline()]);
                }
                catch(const SignatureException& e)
                {
                    printf("    Online validation: FAILED (%s)\n", e.getMsg().c_str());
                }
            }

            // Get signature production place info.
            SignatureProductionPlace spp = sig->getProductionPlace();
            if(!spp.isEmpty())
            {
                printf("    Signature production place:\n");
                printf("      Country:           %s\n", spp.countryName.c_str());
                printf("      State or Province: %s\n", spp.stateOrProvince.c_str());
                printf("      City:              %s\n", spp.city.c_str());
                printf("      Postal code:       %s\n", spp.postalCode.c_str());
            }

            // Get signer role info.
            SignerRole roles = sig->getSignerRole();
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
    digidoc::terminate();

    return 1;
}

/**
 * Create new BDOC container container.
 * Example command: <code>demo create --file=file1.txt --file=file2.txt --profile=TM demo-container.bdoc</code>
 * Args:
 * <ul>
 *   <li><b>--pkcs11Driver</b> - optional, default is '/usr/lib/opensc-pkcs11.so'. Path of PKCS11 driver.</li>
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
    // Initialize digidoc library (initializes also configuration).
    digidoc::initialize();

    std::string pkcs11Driver = Conf::getInstance()->getPKCS11DriverPath();
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
        else if(arg.find("--pkcs11Driver=") == 0)
        {
            pkcs11Driver = arg.substr(15);
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
        DemoEstEIDConsolePinSigner signer(pkcs11Driver);

        // Add signature production place.
        SignatureProductionPlace spp("Tallinn", "Harjumaa", "12345", "Estonia");
        signer.setSignatureProductionPlace(spp);

        // Add signer role(s).
        SignerRole role("CEO");
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
    digidoc::terminate();

    return 1;
}

/**
 * Initialize EstEID signed with driver implementation.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
DemoEstEIDConsolePinSigner::DemoEstEIDConsolePinSigner(const std::string& driver) throw(SignException)
 : EstEIDConsolePinSigner(driver)
{
}

/**
 *
 */
DemoEstEIDConsolePinSigner::~DemoEstEIDConsolePinSigner()
{
}

/**
 * Just show available signing certificates. Uses base class EstEIDSigner implementation
 * to choose the correct signing certificate.
 *
 * NB! If you wan to choose the certificate yourself use PKCS11Signer as base class
 * and implement the <code>selectSigningCertificate()</code> method.
 */
PKCS11Signer::PKCS11Cert DemoEstEIDConsolePinSigner::selectSigningCertificate(
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
 * Prints certificate information.
 *
 * @param cert X.509 certificate.
 */
void DemoEstEIDConsolePinSigner::printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert)
{
    printf("  -------------------------------------------------------\n");
    printf("  token label:        %s\n", cert.token.label.c_str());
    printf("  token manufacturer: %s\n", cert.token.manufacturer.c_str());
    printf("  token model:        %s\n", cert.token.model.c_str());
    printf("  token serial Nr:    %s\n", cert.token.serialNr.c_str());
    printf("  label: %s\n", cert.label.c_str());
}
