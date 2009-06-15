#include "log.h"
#include "BDoc.h"
#include "BDocException.h"
#include "Document.h"
#include "crypto/ocsp/OCSP.h"
#include "crypto/ocsp/OCSPException.h"
#include "crypto/signer/DummySigner.h"
#include "crypto/signer/PKCS11Signer.h"
#include "crypto/signer/SignException.h"
#include "io/IOException.h"
#include "io/ZipSerialize.h"
#include "util/File.h"


#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>


int testException(int argc, char* argv[]);
int testUtil(int argc, char* argv[]);
int testZipSerialize(int argc, char* argv[]);
int testBDOC(int argc, char* argv[]);
int testOCSP(int argc, char* argv[]);
int testSignBDocBES(int argc, char* argv[]);
int testPKCS11Signer(int argc, char* argv[]);



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


    /*
    FILE* pFile = fopen("/home/janno/projects/workspace/DigiDocLIB/etc/TEST-SK.PEM.cer", "r");
    X509* cert = PEM_read_X509(pFile, NULL, NULL, NULL);
    fclose(pFile);

#if 1
    BIO *out = NULL;
    // open BIO for output to 'stdout'
    if (!(out = BIO_new_fp(stdout, BIO_NOCLOSE)))
    {
        ERROR("Error creating OpenSSL BIO");
    }

    X509_print(out, cert);
    BIO_free_all(out);
#endif

    DEBUG("Extracting DER X509");
    //get DER encoded X509 cert
    //internal 2 DER X509
    //temp is needed, see man i2d_X509
    int certLength = i2d_X509(cert, NULL);
    DEBUG("X509 length %d", certLength);

    unsigned char *derEncodedX509 = new unsigned char(certLength);

    certLength = i2d_X509(cert, &derEncodedX509);

    DEBUG("Success. Read %d bytes", certLength);
*/



    try
    {
        return testSignBDocBES(argc, argv);
    }
    catch(const digidoc::BDocException& e)
    {
        ERROR("Caught BDocException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::SignException& e)
    {
        ERROR("Caught SignException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::OCSPException& e)
    {
        ERROR("Caught OCSPException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::IOException& e)
    {
        ERROR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
    {
        ERROR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERROR("Caught unknown exception");
    }

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
	/*
    try
    {
        SSL_load_error_strings();
        SSL_library_init(); //OpenSSL_add_ssl_algorithms(); // makro kutsub välja SSL_library_init()
        //OpenSSL_add_all_digests();
        OPENSSL_config(NULL);


        FILE* pCertFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/test_ocsp_2006.cer", "r");
        X509* cert = PEM_read_X509(pCertFile, NULL, NULL, NULL);
        fclose(pCertFile);

        FILE* pIssuerFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test.pem", "r");
        X509* issuer = PEM_read_X509(pIssuerFile, NULL, NULL, NULL);
        fclose(pIssuerFile);

        //FILE* pIStoreFile = fopen("/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test-ocsp-responder-2005.pem", "r");
        //X509* responderCert = PEM_read_X509(pIStoreFile, NULL, NULL, NULL);
        //fclose(pIStoreFile);
        BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
        ENGINE* e = NULL;
        STACK_OF(X509)* ocspCerts = load_certs(bio_err, "/home/janari/projects/smartlink/digidoc/tmp/php_ocsp_check/etc/sk-test-ocsp-responder-2005.pem", FORMAT_PEM, NULL, e, "validator certificate");


        digidoc::OCSP ocsp("http://www.openxades.org/cgi-bin/ocsp.cgi");
        ocsp.setOCSPCerts(ocspCerts);
        digidoc::OCSP::CertStatus status = ocsp.checkCert(cert, issuer);

        if(status == digidoc::OCSP::GOOD) { DEBUG("OCSP status: GOOD") }
        else if(status == digidoc::OCSP::REVOKED) { DEBUG("OCSP status: REVOKED") }
        else if(status == digidoc::OCSP::UNKNOWN) { DEBUG("OCSP status: UNKNOWN") }
    }
    catch(const digidoc::OCSPException& e)
    {
        ERROR("Caught OCSPException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::IOException& e)
    {
        ERROR("Caught IOException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
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
    return "1111";
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
        digidoc::PKCS11Signer signer("/usr/lib/opensc-pkcs11.so", selectSignCertificate, getPin);

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
        ERROR("Caught SignException: %s", e.getMsg().c_str());
    }
    catch(const digidoc::Exception& e)
    {
        ERROR("Caught Exception: %s", e.getMsg().c_str());
    }
    catch(...)
    {
        ERROR("Caught unknown exception");
    }

    return 0;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int testSignBDocBES(int argc, char* argv[])
{
    try
    {
        //digidoc::DummySigner signer;

        digidoc::PKCS11Signer signer("/usr/lib/opensc-pkcs11.so", selectSignCertificate, getPin);
        digidoc::Signer::SignatureProductionPlace spp("Tallinn", "Harjumaa", "12345", "Estonia");

        digidoc::Signer::SignerRole role("chief o'brian");
        signer.setSignerRole(role);
        signer.setSignatureProductionPlace(spp);

        X509* cert = signer.getCert();
        DEBUG("cert = 0x%X", (unsigned int)cert)

        digidoc::BDoc bdoc;
        //std::auto_ptr<digidoc::ISerialize> serializer(new digidoc::ZipSerialize("/home/janari/projects/smartlink/digidoc/tmp/digidoc.zip"));
        //bdoc.readFrom(serializer);

        digidoc::Document doc_001("/proc/cpuinfo", "text/txt");
        bdoc.addDocument(doc_001);

        //bdoc.sign(&signer, digidoc::Signature::BES);
        const_cast<digidoc::Signature*>(bdoc.getSignature(bdoc.signatureCount() - 1))->saveToXml();
     }
     catch(const digidoc::SignException& e)
     {
         ERROR("Caught SignException: %s", e.getMsg().c_str());
     }
     catch(const digidoc::Exception& e)
     {
         ERROR("Caught Exception: %s", e.getMsg().c_str());
     }
     catch(...)
     {
         ERROR("Caught unknown exception");
     }

    // save/print out the signature created.


	return 0;
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

    std::auto_ptr<digidoc::ISerialize> serializer(new digidoc::ZipSerialize(fileName));
    digidoc::BDoc bdoc;
    //bdoc.readFrom(serializer);

    return 0;
};
