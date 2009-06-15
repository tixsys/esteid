#include "log.h"
#include "BDoc.h"
#include "BDocException.h"
#include "Conf.h"
#include "Document.h"
#include "crypto/Digest.h"
#include "crypto/cert/X509CertStore.h"
#include "crypto/cert/DirectoryX509CertStore.h"
#include "crypto/ocsp/OCSP.h"
#include "crypto/ocsp/OCSPException.h"
#include "crypto/signer/DummySigner.h"
#include "crypto/signer/PKCS11Signer.h"
#include "crypto/signer/SignException.h"
#include "SignatureException.h"
#include "io/IOException.h"
#include "io/ZipSerialize.h"
#include "util/File.h"
#include "xml/OpenDocument_manifest.hxx"

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <iostream>

#include <xercesc/dom/DOM.hpp>

#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

using namespace digidoc;


int testException(int argc, char* argv[]);
int testUtil(int argc, char* argv[]);
int testZipSerialize(int argc, char* argv[]);
int testBDOC(int argc, char* argv[]);
int testOCSP(int argc, char* argv[]);
int testOpenBDocBES(int argc, char* argv[]);
int testShowCertInfo(const digidoc::Signature* signature);
int testSignBDocBES(int argc, char* argv[]);
int testSignBDocTM(int argc, char* argv[]);
int testPKCS11Signer(int argc, char* argv[]);
int testC14N(int argc, char* argv[]);
void dumpException(const digidoc::Exception& e);
int testTMValidate();



/**
 * Implements cert selection and PIN acquisition.
 */
class PKCS11ConsolePinSigner : public PKCS11Signer
{

  public:
	  PKCS11ConsolePinSigner(const std::string& driver) throw(SignException);
      virtual ~PKCS11ConsolePinSigner();

  protected:
      virtual PKCS11Signer::PKCS11Cert selectSigningCertificate(std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException);
      virtual std::string getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException);

  private:
      void printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert);

};


/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    SSL_load_error_strings();
    SSL_library_init(); //OpenSSL_add_ssl_algorithms(); // makro kutsub välja SSL_library_init()
    //OpenSSL_add_all_digests();
    OPENSSL_config(NULL);

	xercesc::XMLPlatformUtils::Initialize();
    XSECPlatformUtils::Initialise();

    digidoc::X509CertStore* store = new digidoc::DirectoryX509CertStore(std::string("etc/certs"));
    digidoc::X509CertStore::init(store);

    try
    {
    	/*
    	digidoc::SHA1Digest d;
    	unsigned char b[1] = { 0 };
    	d.update(&b[0], 0);
    	std::vector<unsigned char> sha1 = d.getDigest();
    	DEBUG("sha1.size() = %d", sha1.size())
    	xml_schema::Base64Binary bin(&sha1[0], 20);
    	std::cout << bin.encode();
    	*/


    	//std::vector<std::string> files = digidoc::util::File::listFiles("/home/janari/projects/smartlink/digidoc/tmp/aaaa", false, true, false);
    	//DEBUG("------------------------------------------------------------------------------------")
    	//for(std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
    	//{
        //    DEBUG("%s", iter->c_str())
    	//}
    	//DEBUG("------------------------------------------------------------------------------------")

    	//test();
    	//return testOCSP(argc, argv);
        //return testSignBDocBES(argc, argv);
        //return testSignBDocTM(argc, argv);
        //return testSignBDocBES(argc, argv);
    	//return testC14N(argc, argv);
        return testOpenBDocBES(argc, argv);
        //return testOCSP(argc, argv);
        //return testTMValidate();
    }
    catch(const digidoc::BDocException& e)
    {
        ERR("Caught BDocException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::SignException& e)
    {
        ERR("Caught SignException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::OCSPException& e)
    {
        ERR("Caught OCSPException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::IOException& e)
    {
        ERR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
    {
        ERR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERR("Caught unknown exception");
    }

    XSECPlatformUtils::Terminate();
	xercesc::XMLPlatformUtils::Terminate();

    return -1;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testException(int argc, char* argv[])
{
    /*
    digidoc::IOException ioe(__FILE__, __LINE__, "IOE");
    digidoc::BDocException be(__FILE__, __LINE__, "BE", ioe);

    DEBUG("be.hasCause(); %i", be.hasCause())
    if(be.hasCause())
    {
        DEBUG("%s", be.getCause().getMsg().c_str())
    }

    digidoc::BDocException e(be);
    DEBUG("e  = 0x%X", (unsigned int)&e);
    DEBUG("be = 0x%X", (unsigned int)&be);
    DEBUG("%s", e.getCause().getMsg().c_str())
    */

    return 0;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testUtil(int argc, char* argv[])
{
    //bool exists = digidoc::Util::File::fileExists("/tmp/Security.zip");
    //bool exists = digidoc::Util::File::directoryExists("/home/janari/");
    //DEBUG("exists = %d", exists);

    //std::string dir = digidoc::Util::File::directory("/home/janari/projects");
    //std::string str = digidoc::Util::File::tempFileName();
    //std::string str = digidoc::Util::File::path("/tmp/asaasasasx/", "sssssx\\sssx/kkkkx\\text.txt");
    //std::string str = digidoc::Util::File::path("\\ffff\\dddd/jjj", "sssssx\\sssx/kkkkx\\text.txt");
    //DEBUG("str = '%s'", str.c_str())
    //unsigned long size =  digidoc::Util::File::fileSize("/tmp/CHIFFR~1-1.ZIP");
    //DEBUG("size = %lu", size)
    //digidoc::Util::File::createDirectory("/tmp/test-003/test/blaaah/hjhjh/ddd/");
    //digidoc::Util::File::moveFile("/tmp/BDoc-1.0.pdf___", "/tmp/BDoc-1.0.pdf");
    //digidoc::Util::File::copyFile("/tmp/BDoc-1.0.pdf", "/tmp/BDoc-1.0.pdf__");
    /*
    std::vector<std::string> files = digidoc::Util::File::listFiles("/tmp", true);
    DEBUG("--------------------------------------------------------------------------------------")
    for(std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
    {
        DEBUG("%s", iter->c_str())
    }
    DEBUG("--------------------------------------------------------------------------------------")
    */

    return 0;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testZipSerialize(int argc, char* argv[])
{
    /*
    try
    {
        digidoc::ZipSerialize zip("digidoc.zip");

        //zip.extract();

        zip.addFile("test.txt", "mimetype");
        zip.addFile("document.doc", "document.doc");
        zip.addFile("META-INF/test.txt", "mimetype");
        zip.save();
    }
    catch(const digidoc::IOException e)
    {
        ERROR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception e)
    {
        ERROR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERROR("Caught unknown exception");
    }
    */

    return 0;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testBDOC(int argc, char* argv[])
{
	/*
    try
    {
        digidoc::BDoc bdoc;
        std::auto_ptr<digidoc::ISerialize> serializer(new digidoc::ZipSerialize("digidoc.zip"));
        //bdoc.readFrom(serializer);

        digidoc::Document doc_001("document1.doc", "word");
        digidoc::Document doc_002("document2.doc", "word");
        digidoc::Document doc_003("document3.doc", "word");

        bdoc.addDocument(doc_001);
        bdoc.addDocument(doc_002);
        bdoc.addDocument(doc_003);

        digidoc::Signature* sig_001 = new digidoc::Signature("META-INF/signature1.xml", "signature/bdoc-1.0/BES");
        digidoc::Signature* sig_002 = new digidoc::Signature("META-INF/signature2.xml", "signature/bdoc-1.0/TS");
        digidoc::Signature* sig_003 = new digidoc::Signature("META-INF/signature3.xml", "signature/bdoc-1.0/TM");
        digidoc::Signature* sig_004 = new digidoc::Signature("META-INF/signature4.xml", "signature/bdoc-1.0/BES");

        bdoc.addSignature(sig_001);
        bdoc.addSignature(sig_002);
        bdoc.addSignature(sig_003);
        bdoc.addSignature(sig_004);

        //for(unsigned int i = 0; i < bdoc.documentCount(); i++)
        //{
        //    digidoc::Document doc = bdoc.getDocument(i);
        //    std::string msg;
        //    msg += doc.getFullPath() + std::string(" :: ") + doc.getMediaType() + std::string("\n");
        //    printf(msg.c_str());
        //}

        //bdoc.save();
        bdoc.saveTo(serializer);

        //bdoc.getDocument(100);
    }
    catch(const digidoc::BDocException e)
    {
        ERROR("Caught BDocException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::IOException e)
    {
        ERROR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception e)
    {
        ERROR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERROR("Caught unknown exception");
    }
    */

    return 0;
}


#include <openssl/ssl.h>
#include <openssl/conf.h>


typedef struct pw_cb_data
    {
    const void *password;
    const char *prompt_info;
    } PW_CB_DATA;


#define FORMAT_PEM 3


STACK_OF(X509) *load_certs(BIO *err, const char *file, int format,
    const char *pass, ENGINE *e, const char *cert_descrip)
    {
    BIO *certs;
    int i;
    STACK_OF(X509) *othercerts = NULL;
    STACK_OF(X509_INFO) *allcerts = NULL;
    X509_INFO *xi;
    PW_CB_DATA cb_data;

    cb_data.password = pass;
    cb_data.prompt_info = file;

    if((certs = BIO_new(BIO_s_file())) == NULL)
        {
        //ERR_print_errors(err);
        goto end;
        }

    if (file == NULL)
        BIO_set_fp(certs,stdin,BIO_NOCLOSE);
    else
        {
        if (BIO_read_filename(certs,file) <= 0)
            {
            BIO_printf(err, "Error opening %s %s\n",
                cert_descrip, file);
            //ERR_print_errors(err);
            goto end;
            }
        }

    if      (format == FORMAT_PEM)
        {
        othercerts = sk_X509_new_null();
        if(!othercerts)
            {
            sk_X509_free(othercerts);
            othercerts = NULL;
            goto end;
            }
        allcerts = PEM_X509_INFO_read_bio(certs, NULL,
                NULL/*(pem_password_cb *)password_callback*/, &cb_data);
        for(i = 0; i < sk_X509_INFO_num(allcerts); i++)
            {
            xi = sk_X509_INFO_value (allcerts, i);
            if (xi->x509)
                {
                sk_X509_push(othercerts, xi->x509);
                xi->x509 = NULL;
                }
            }
        goto end;
        }
    else    {
        BIO_printf(err,"bad input format specified for %s\n",
            cert_descrip);
        goto end;
        }
end:
    if (othercerts == NULL)
        {
        BIO_printf(err,"unable to load certificates\n");
        //ERR_print_errors(err);
        }
    if (allcerts) sk_X509_INFO_pop_free(allcerts, X509_INFO_free);
    if (certs != NULL) BIO_free(certs);
    return(othercerts);
    }


/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testOCSP(int argc, char* argv[])
{
	try
    {
		/*
        SSL_load_error_strings();
        SSL_library_init(); //OpenSSL_add_ssl_algorithms(); // makro kutsub välja SSL_library_init()
        //OpenSSL_add_all_digests();
        OPENSSL_config(NULL);
        */

        //FILE* pCertFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/test_ocsp_2006.cer", "r");
        //FILE* pCertFile = fopen("etc/certs/test_ocsp_2006.cer", "r");
        FILE* pCertFile = fopen("etc/certs/mari-liis.mannik.pem", "r");
        X509* cert = PEM_read_X509(pCertFile, NULL, NULL, NULL);
        fclose(pCertFile);

        //FILE* pIssuerFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test.pem", "r");
        FILE* pIssuerFile = fopen("etc/certs/sk-test.pem", "r");
        X509* issuer = PEM_read_X509(pIssuerFile, NULL, NULL, NULL);
        fclose(pIssuerFile);

        //FILE* pIStoreFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test-ocsp-responder-2005.pem", "r");
        //X509* responderCert = PEM_read_X509(pIStoreFile, NULL, NULL, NULL);
        //fclose(pIStoreFile);
        BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
        ENGINE* e = NULL;
        //STACK_OF(X509)* ocspCerts = load_certs(bio_err, "/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test-ocsp-responder-2005.pem", FORMAT_PEM, NULL, e, "validator certificate");
        STACK_OF(X509)* ocspCerts = load_certs(bio_err, "etc/certs/sk-test-ocsp-responder-2005.pem", FORMAT_PEM, NULL, e, "validator certificate");

        digidoc::OCSP ocsp("http://www.openxades.org/cgi-bin/ocsp.cgi");
        //ocsp.setOCSPCerts(ocspCerts);
        std::vector<unsigned char> nonce(20);
        ocsp.setMaxAge(5);
        std::vector<unsigned char> ocspResponseDER;
        tm producedAt;
        digidoc::OCSP::CertStatus status = ocsp.checkCert(cert, issuer, nonce, ocspResponseDER, producedAt);

        if(status == digidoc::OCSP::GOOD) { DEBUG("OCSP status: GOOD"); }
        else if(status == digidoc::OCSP::REVOKED) { DEBUG("OCSP status: REVOKED"); }
        else if(status == digidoc::OCSP::UNKNOWN) { DEBUG("OCSP status: UNKNOWN"); }
    }
    catch(const digidoc::OCSPException& e)
    {
        ERR("Caught OCSPException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::IOException& e)
    {
        ERR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
    {
        ERR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERR("Caught unknown exception");
    }

    return 0;
}


X509* selectSignCertificate(std::vector<digidoc::PKCS11Signer::PKCS11Cert> certificates)
{
	INFO("selectSignCertificate(certificates.size() = %d)", certificates.size())

	for(std::vector<digidoc::PKCS11Signer::PKCS11Cert>::const_iterator iter = certificates.begin(); iter != certificates.end(); iter++)
	{
        DEBUG("token label:        %s", iter->token.label.c_str())
        DEBUG("token manufacturer: %s", iter->token.manufacturer.c_str())
        DEBUG("token model:        %s", iter->token.model.c_str())
        DEBUG("token serial Nr:    %s", iter->token.serialNr.c_str())
        DEBUG("label: %s", iter->label.c_str())
        DEBUG("cert: 0x%X", (unsigned int)(iter->cert))
        DEBUG("--------------------------------------------------------------------")
	}

	return certificates[0].cert;
}

std::string getPin(digidoc::PKCS11Signer::PKCS11Cert certificate)
{
	INFO("getPin(certificate = { token = {label = '%s'}})", certificate.token.label.c_str())
    return "0090";
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testPKCS11Signer(int argc, char* argv[])
{
    try
    {
        PKCS11ConsolePinSigner signer("/usr/lib/opensc-pkcs11.so");
        //digidoc::PKCS11Signer signer("C:\\dev\\tools\\Copy of opensc-0.11.6\\src\\pkcs11\\opensc-pkcs11.dll", selectSignCertificate, getPin);

        digidoc::Signer::SignatureProductionPlace spp("Tallinn", "Harjumaa", "12345", "Estonia");
        signer.setSignatureProductionPlace(spp);

        digidoc::Signer::SignerRole role("chief o'brian");
        signer.setSignerRole(role);

        X509* cert = signer.getCert();
        DEBUG("cert = 0x%X", (unsigned int)cert)

        digidoc::Signer::Digest digest = { NID_sha1, (unsigned char*)"ABCDEFGHIJ0123456789", 20 };
        digidoc::Signer::Signature signature = { new unsigned char[256], 256 };
        memset(signature.signature, 0, signature.length);
        //digidoc::Signer::Signature signature = { NULL, 0 };
        try { signer.sign(digest, signature); } catch(const digidoc::SignException& ) {}

        DEBUG("----------------------------------------------------------------------------------------------------")
        DEBUG("Digest: type = %d, digest = %s, length = %d", digest.type, (char*)digest.digest, digest.length)
        DEBUG("Signature: signature = 0x%X, length = %d", (unsigned int)signature.signature, signature.length)

        //DEBUG("%u", (signature.signature)[0])
        DEBUG("signature.signature[100] = %u", (signature.signature)[100])
        DEBUG("signature.signature[127] = %u", (signature.signature)[127])
        DEBUG("signature.signature[128] = %u", (signature.signature)[128])
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

    return 0;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testSignBDoc(digidoc::Signature::Type signingType, int argc, char* argv[])
{
    try
    {
        //FILE* pCertFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test.pem", "r");
        //X509* x509 = PEM_read_X509(pCertFile, NULL, NULL, NULL);
        //fclose(pCertFile);
        //digidoc::DummySigner signer(x509);

    	PKCS11ConsolePinSigner signer("/usr/lib/opensc-pkcs11.so");
        digidoc::Signer::SignatureProductionPlace spp("Tallinn", "Harjumaa", "12345", "Estonia");

        digidoc::Signer::SignerRole role("chief o'brian");
        signer.setSignerRole(role);
        signer.setSignatureProductionPlace(spp);

        X509* cert = signer.getCert();
        DEBUG("cert = 0x%X", (unsigned int)cert)

        //std::auto_ptr<digidoc::ISerialize> serializer(new digidoc::ZipSerialize("/home/janari/projects/smartlink/digidoc/tmp/digidoc.zip"));
        digidoc::BDoc bdoc;
        //bdoc.readFrom(serializer);

        if(argc == 3)
        {
            DEBUG("args: %s %s", argv[1], argv[2]);
            digidoc::Document doc_001(argv[1], argv[2]);
            bdoc.addDocument(doc_001);
        }
        else
        {
            digidoc::Document doc_001("/proc/cpuinfo", "text/txt");
            bdoc.addDocument(doc_001);
        }

        bdoc.sign(&signer, signingType);

        // save/print out the signature created.
        digidoc::Signature* sig = const_cast<digidoc::Signature*>(bdoc.getSignature(bdoc.signatureCount() - 1));
        sig->saveToXml();

        //sig->validateOnline();

        //bdoc.saveTo(serializer);
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

    return 0;
}

int testSignBDocBES(int argc, char* argv[])
{
    return testSignBDoc(digidoc::Signature::BES, argc, argv);
}

int testSignBDocTM(int argc, char* argv[])
{
    return testSignBDoc(digidoc::Signature::TM, argc, argv);
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testOpenBDocBES(int argc, char* argv[])
{
    const std::string fileName("doc1.bdoc");
    DEBUG(fileName.c_str());

    //testSignBDocBES();
    //testBDOC(argc, argv);
    //testPKCS11Signer(argc, argv);

    std::auto_ptr<digidoc::ISerialize> serializer(new digidoc::ZipSerialize(fileName));
    std::auto_ptr<digidoc::BDoc> bdoc(new digidoc::BDoc(serializer));

    size_t sigCount = bdoc->signatureCount();
    for ( size_t sig = 0; sig < sigCount; sig++ )
    {
        const digidoc::Signature* signature = bdoc->getSignature(sig);
        std::cout << "Signature[" << sig << "]:" << std::endl;

        try
        {
            signature->validateOffline();
            std::cout << "\tofflineValid[true]" << std::endl;
        }
        catch (digidoc::SignatureException& e)
        {
            dumpException(e);
        }

        digidoc::Signer::SignatureProductionPlace productionPlace = signature->getProductionPlace();
        std::cout << "\tSignatureProductionPlace:" << std::endl;
        std::cout << "\t\tcity[" << productionPlace.city << "]" << std::endl;
        std::cout << "\t\tstateOrProvince[" << productionPlace.stateOrProvince << "]" << std::endl;
        std::cout << "\t\tpostalCode[" << productionPlace.postalCode << "]" << std::endl;
        std::cout << "\t\tcountryName[" << productionPlace.countryName << "]" << std::endl;

        digidoc::Signer::SignerRole signerRole = signature->getSignerRole();
        std::cout << "\tClaimedRoles:" << std::endl;
        for ( digidoc::Signer::SignerRole::TRoles::const_iterator it = signerRole.claimedRoles.begin()
            ; it != signerRole.claimedRoles.end()
            ; it++ )
        {
            std::cout << "\t\tClaimedRole[" << (*it) << "]" << std::endl;
        }

        std::cout << "\tSigningTime:" << std::endl;
        std::cout << "\t\ttime[" << signature->getSigningTime() << "]" << std::endl;

        // certs
        testShowCertInfo(signature);
    }




    return 0;
};

int testTMValidate()
{
    try
    {
        PKCS11ConsolePinSigner signer("/usr/lib/opensc-pkcs11.so");
        digidoc::Signer::SignatureProductionPlace spp("Tallinn", "Harjumaa", "12345", "Estonia");

        digidoc::Signer::SignerRole role("chief o'brian");
        signer.setSignerRole(role);
        signer.setSignatureProductionPlace(spp);

        digidoc::BDoc bdoc;

        digidoc::Document doc_001("/proc/cpuinfo", "text/txt");
        bdoc.addDocument(doc_001);


        bdoc.sign(&signer, digidoc::Signature::TM);

        // save/print out the signature created.
        digidoc::Signature* sig = const_cast<digidoc::Signature*>(bdoc.getSignature(bdoc.signatureCount() - 1));
        //sig->saveToXml();

        sig->validateOffline();
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

    return 0;

}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testC14N(int argc, char* argv[])
{
	try
	{
		//xercesc::XMLPlatformUtils::Initialize();
        //XSECPlatformUtils::Initialise();

        DEBUG("POS[10]")
		std::string fileName = "/home/janari/projects/smartlink/digidoc/tmp/example_container/META-INF/manifest.xml";
		xml_schema::Properties properties;
	    properties.schema_location("urn:oasis:names:tc:opendocument:xmlns:manifest:1.0", digidoc::Conf::getInstance()->getManifestXsdPath());
	    std::auto_ptr<digidoc::manifest::Manifest> manifest(digidoc::manifest::manifest(fileName, xml_schema::Flags::keep_dom, properties));

        DEBUG("POS[20]")

	    xercesc::DOMNode* pNode = manifest->_node();
	    /*
	    //xercesc::DOMDocument* pDoc = new xercesc::DOMDocument();
        //xercesc::DOMDocument* pDoc = NULL;
        DEBUG("POS[21]")

        //DEBUG("POS[22] :: DOMImplementation::getImplementation() = 0x%X", (unsigned int)xercesc::DOMImplementation::getImplementation())
	    //xercesc::DOMDocument* pDoc = xercesc::DOMImplementation::getImplementation()->createDocument();

        const XMLCh ls_id [] = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};
        DEBUG("POS[22]")
        xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(ls_id);
        DEBUG("POS[23] :: impl = 0x%X", (unsigned int)impl)


        xercesc::DOMDocument* pDoc = xercesc::DOMImplementationRegistry::getDOMImplementation(ls_id)->createDocument();

        DEBUG("POS[26] :: pNode = 0x%X", (unsigned int)pNode)
	    //pDoc->adoptNode(pNode);
        pDoc->importNode(pNode, true);
        */

        DEBUG("POS[30]")
        //DEBUG("POS[31] :: pNode->getOwnerDocument() = 0x%X", (unsigned int)pNode->getOwnerDocument())
	    XSECC14n20010315 canon(pNode->getOwnerDocument() /*pDoc*/);
	   	canon.setCommentsProcessing(true);
	   	canon.setUseNamespaceStack(true);

	   	canon.setStartNode(manifest->file_entry()[0]._node());

        DEBUG("POS[40]")
		char buffer[512];
		int res = canon.outputBuffer((unsigned char *) buffer, 128);
		DEBUG("POS[41] :: res = %d", res)
		while(res != 0)
		{
			buffer[res] = '\0';
			std::cout << buffer;
			res = canon.outputBuffer((unsigned char *) buffer, 128);
		}

		std::cout << std::endl;
        DEBUG("POS[60]")


        //XSECPlatformUtils::Terminate();
		//xercesc::XMLPlatformUtils::Terminate();
	}
	catch(const xercesc::DOMException& e)
	{
        ERR("xercesc::DOMException: %s", XMLString::transcode(e.getMessage()));
	}
	catch(const xercesc::XMLException& e)
	{
        ERR("xercesc::DOMException: %s", XMLString::transcode(e.getMessage()));
	}
    catch(const xml_schema::Exception& e)
    {
        std::ostringstream oss;
        oss << e;
        ERR("xml_schema::Exception: %s", oss.str().c_str());
    }


    return 0;
}

int testShowCertInfo(const digidoc::Signature* signature)
{
    digidoc::X509Cert cert(signature->getSigningCertificate());

    std::cout << "\tCertificateInfo:" << std::endl;
    std::cout << "\t\tserial[" << cert.getCertSerial() << "]" << std::endl;
    std::cout << "\t\tissuerName[" << cert.getCertIssuerName() << "]" << std::endl;

    return 0;
}

std::string indent(size_t level, char character = ' ')
{
    std::string str;
    str.resize(level, character);
    return str;
}

void getExceptionMsg(const digidoc::Exception& e
                     , std::stringstream& addTo
                     , size_t depth
                     )
{
    addTo << indent(depth)
        << "Exception[" << e.getMsg() << "]"
        << std::endl;

    if ( e.hasCause() )
    {
        digidoc::Exception::Causes causedBy = e.getCauses();

        addTo << indent(depth + 1) << "Caused by:" << std::endl;
        for ( digidoc::Exception::Causes::const_iterator it = causedBy.begin()
            ; it != causedBy.end()
            ; it++ )
        {
             getExceptionMsg(*it, addTo, depth + 1); // recurse
        }

    }
}

void dumpException(const digidoc::Exception& e)
{
    std::stringstream stream;
    getExceptionMsg(e, stream, 0);
    std::cout <<  stream.str() << std::endl;
}



/**
 * Implements cert selection and PIN acquisition.
 */
PKCS11ConsolePinSigner::PKCS11ConsolePinSigner(const std::string& driver) throw(SignException)
 : PKCS11Signer(driver)
{
}

PKCS11ConsolePinSigner::~PKCS11ConsolePinSigner()
{
}

/**
 * Show available signing certificates and choose the one used to sign.
 */
PKCS11Signer::PKCS11Cert PKCS11ConsolePinSigner::selectSigningCertificate(
		std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException)
{
    // Print available certificates.
    DEBUG("Available certificates:\n");
    for(std::vector<PKCS11Signer::PKCS11Cert>::const_iterator iter = certificates.begin(); iter != certificates.end(); iter++)
    {
        printPKCS11Cert(*iter);
    }

    // 0 - Isikutuvastus, 1 - Allkirjastamine
    PKCS11Signer::PKCS11Cert cert = certificates[0];
    DEBUG("Selected certificate: %s", cert.token.label.c_str());
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
std::string PKCS11ConsolePinSigner::getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException)
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
    //return "0090";
    //return "01497";
}

/**
 * Prints certificate information.
 *
 * @param cert X.509 certificate.
 */
void PKCS11ConsolePinSigner::printPKCS11Cert(const PKCS11Signer::PKCS11Cert& cert)
{
	DEBUG("-------------------------------------------------------");
	DEBUG("  token label:        %s", cert.token.label.c_str());
	DEBUG("  token manufacturer: %s", cert.token.manufacturer.c_str());
	DEBUG("  token model:        %s", cert.token.model.c_str());
	DEBUG("  token serial Nr:    %s", cert.token.serialNr.c_str());
	DEBUG("  label: %s", cert.label.c_str());
}
