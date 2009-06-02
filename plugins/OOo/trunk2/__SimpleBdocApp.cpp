#include <digidoc/BDoc.h>
#include <digidoc/log.h>
#include <digidoc/Conf.h>
#include <digidoc/SignatureBES.h>//###
#include <digidoc/crypto/cert/DirectoryX509CertStore.h>
#include <digidoc/crypto/cert/X509Cert.h>
#include <digidoc/crypto/signer/EstEIDSigner.h>
#include <digidoc/io/ZipSerialize.h>

#include <digidoc/crypto/signer/PKCS11Signer.h>
#include "SimpleBdocApp.h"

#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>
#include <iostream>

#ifndef BDOCLIB_CONF_PATH
#define BDOCLIB_CONF_PATH "bdoclib.conf"
#endif

using namespace digidoc;
using namespace std;

const char* CERT_STATUSES[3] = { "GOOD", "REVOKED", "UNKNOWN" };

//********************************************
//--------------------------------------------
// Constructor for My EstEIDSigner class
MyEstEIDSigner::MyEstEIDSigner() throw(SignException)
:	EstEIDSigner( Conf::getInstance()->getPKCS11DriverPath() )
{
	
	cardSignCert = NULL;
	try	
	{
		cardSignCert = getCert();	
	}
	catch( const Exception & ) {}	
}
//--------------------------------------------
//********************************************
//--------------------------------------------
// Destructor  in MyEstEIDSigner
MyEstEIDSigner::~MyEstEIDSigner() 
{
}
//--------------------------------------------
//********************************************
//--------------------------------------------
// Function GetPin  in MyEstEIDSigner
std::string MyEstEIDSigner::getPin( PKCS11Cert certificate ) throw(SignException)
{	
/*	std::string pin ="";
	cout << endl << "Enter PIN2: ";
	for(int u=0; u<5; u++)
	{
		pin += _getch();
		cout << "*";
		
	}
*/	
	return pin;
}
//--------------------------------------------
//********************************************
//--------------------------------------------
// Function initData in SimpleBdocApp
// - Use this function before creating a new Bdoc container
// - Do all initialization, set flags, etc. here
int SimpleBdocApp::initData ()
{
	//***set installed BDoc library path
	char *val = getenv( "BDOCLIB_CONF_XML" );
	if( val == 0 )
	{
		char *conf = "BDOCLIB_CONF_XML=" BDOCLIB_CONF_PATH;
		putenv( conf );
		val = getenv( "BDOCLIB_CONF_XML" );
	}
	//***	
	digidoc::initialize();
	//std::string pkcs11Driver = Conf::getInstance()->getPKCS11DriverPath();	//--already in MyEstEIDSigner constructor 
	//std::vector<std::string> files;	//--we are using only one file to sign in OOo
	//str_origin = str_filepath;
	profile = Signature::TM;//BES
	//str_destination = str_bdocpath;	
	validateOnline = true;
	
	locBdoc = 0;
	return 0; 
}
//--------------------------------------------
//********************************************
//--------------------------------------------
// Function signFile in SimpleBdocApp
int SimpleBdocApp::signFile ()
{
	int i_ok = 0; //for testing
	
	try
	{
		
		MyEstEIDSigner m_signer;
		m_signer.pin = str_pin;
		// Init certificate store.
		X509CertStore::init( new DirectoryX509CertStore() );
	//	cout << m_signer.cardSignCert->name<<endl;
	//	_getch();
		if (locBdoc)
		{
			digidoc::initialize();
			
			if (!compIDnumber(m_signer.cardSignCert->name))
				i_ok = 11;
		}		
		else //if it's an new container (in openoffice we will have only 1 file per container)
		{
			locBdoc = new BDoc();
			locBdoc->addDocument(Document(str_filepath, "file"));
		}

		if (!i_ok)
		{
			// Add signature production place.
			Signer::SignatureProductionPlace spp(signPlace.str_city, signPlace.str_stateOrProvince, signPlace.str_postalCode, signPlace.str_countryName);
       		m_signer.setSignatureProductionPlace(spp);

			// Add signer role(s).
			Signer::SignerRole role(signerRoles.str_role);
			// Add additional roles.
			role.claimedRoles.push_back(signerRoles.str_additionalRole);
			m_signer.setSignerRole(role);

			// Sign the BDOC container.
			locBdoc->sign(&m_signer, profile);
			
			// Save the BDOC container.
			std::auto_ptr<ISerialize> serializer(new ZipSerialize(str_bdocpath));
			locBdoc->saveTo(serializer);

			// Destroy certificate store.
			X509CertStore::destroy();
		}	
    }
	
    catch(const digidoc::BDocException& e)
    {
        ERR("Caught BDocException: %s", e.getMsg().c_str());
		i_ok = 1;
    }
    catch(const digidoc::IOException& e)
    {
        ERR("Caught IOException: %s", e.getMsg().c_str());
		i_ok = 2;
    }
    catch(const digidoc::SignException& e)
    {
        ERR("Caught SignException: %s", e.getMsg().c_str());
		i_ok = 3;
    }
    catch(const digidoc::Exception& e)
    {
        ERR("Caught Exception: %s", e.getMsg().c_str());
		i_ok = 4;
    }
    catch(...)
    {
        ERR("Caught unknown exception");
		i_ok = 9;
    }

	// Terminate digidoc library.
    digidoc::terminate();
	
	// free memory
	
	//delete m_signer;	//***** Mingi JAMA!!! paneb pange kui kustudada
	//m_signer = 0;		// (kui olemasolevat cont allkirjastada ja isikukood on juba konteineris olemas)
	delete locBdoc;
	locBdoc = 0;
	 
	return i_ok;
}
//--------------------------------------------
//********************************************
//--------------------------------------------
// Function openCont in SimpleBdocApp
int SimpleBdocApp::openCont()
{	
    try
    {
		// Init certificate store.
        X509CertStore::init(new DirectoryX509CertStore());

		// Open BDOC container.
        std::auto_ptr<ISerialize> serializer(new ZipSerialize(str_bdocpath));
        locBdoc = new BDoc(serializer);		

		//we assume that we have only one file in container (for openoffice)
		Document file = locBdoc->getDocument(0);
		str_filepath = file.getPath().c_str();
	//	cout << endl << str_origin << endl; 
		
        // Print container signatures list.
//		printf("\nSignatures:\n");
        for(unsigned int i = 0; i < locBdoc->signatureCount(); i++)
		{
            
			// Get signature from bdoc cotainer.
            const Signature* sig = locBdoc->getSignature(i);
//		    printf("  Signature %u (%s):\n", i, sig->getMediaType().c_str());

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
            Signer::SignatureProductionPlace spp = sig->getProductionPlace();
            if(!spp.isEmpty())
            {
            	signPlace.str_city = spp.city.c_str();
				signPlace.str_stateOrProvince = spp.stateOrProvince.c_str();
				signPlace.str_postalCode = spp.postalCode.c_str();
				signPlace.str_countryName = spp.countryName.c_str();
/*
				cout << "********************************************"  << endl;
				cout << "*   City:            " << signPlace.str_city << endl;
				cout << "*   State/Province:  " << signPlace.str_stateOrProvince << endl;
				cout << "*   Postal/ZIP:      " << signPlace.str_postalCode << endl;
				cout << "*   Country:         " << signPlace.str_countryName << endl;
				cout << "********************************************"  << endl;
 */           }

            // Get signer role info.
            Signer::SignerRole roles = sig->getSignerRole();
            if(!roles.isEmpty())
            {
            	signerRoles.str_role = roles.claimedRoles.at(0);
				signerRoles.str_additionalRole = roles.claimedRoles.at(1);
/*				cout << "*   " << signerRoles.str_role << endl;
				cout << "*   " << signerRoles.str_additionalRole << endl;
				cout << "********************************************"  << endl;
  */          }

            // Get signing time.
			str_signTime = sig->getSigningTime().c_str();
//            cout << "*   Signing time: " << str_signTime << endl;
//			cout << "********************************************"  << endl;

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
	
    return 0;
}	
//--------------------------------------------
//********************************************
//--------------------------------------------
// Function compIDnumber in SimpleBdocApp
// - checks if container has a signature with same personID
// as in inserted ID card
bool SimpleBdocApp::compIDnumber(std::string str_idNum)
{
	string str_cerNum, id1, id2;
	int k = 0;
	size_t found;
	const Signature* sign;

	found=str_idNum.find("serialNumber=");
	if (found!=string::npos)
	{
		id1.assign(str_idNum, int(found), 24);
//		cout << "id1: " << id1 << endl;
	}

	for(unsigned int i = 0; i < locBdoc->signatureCount(); i++)
	{
		sign = locBdoc->getSignature(i);
		
		singnCert = new X509Cert(sign->getSigningCertificate());
		
		str_cerNum = singnCert->getSubject(); 
		
		found=str_cerNum.find("serialNumber=");
		if (found!=string::npos)
		{
			id2.assign(str_cerNum, int(found), 24);
//			cout << "id2: " << id2 << endl;
		}
		delete singnCert; 
		singnCert = 0;

		if(!id1.compare(id2))
			return false;
	}		
	
	return true;
}
//--------------------------------------------
//********************************************
//--------------------------------------------
/*/Main program Just for testing
int main()
{	
	SimpleBdocApp cl_MyBdocApp;
	char a = 0;
	cl_MyBdocApp.str_bdocpath = "C:/Documents and Settings/Mark/Desktop/test-bin/bluff.bdoc";
	cl_MyBdocApp.str_filepath = "C:/Documents and Settings/Mark/Desktop/test-bin/muff.txt";	
	
	printf("type:\n     \"i\" - for init (start new process)\n     \"s\" - for signing\n     \"o\" - for opening container\n     \"e\" - for exit program\n");
	
	cl_MyBdocApp.initData();	

	do
	{
		printf("\nYour choise:");
		a = _getch();
		cout << a << endl;
		
		switch (a)
		{
			case 'i':
				cl_MyBdocApp.initData();
				break;

			case 's':				
				cout << endl << "City: ";
				cin >> cl_MyBdocApp.signPlace.str_city;
				cout << endl << "State: ";
				cin >> cl_MyBdocApp.signPlace.str_stateOrProvince;
				cout << endl << "Postal/zip: ";
				cin >> cl_MyBdocApp.signPlace.str_postalCode;
				cout << endl << "Country: ";
				cin >> cl_MyBdocApp.signPlace.str_countryName;
				
				cout << endl << "Role/Position: ";
				cin >> cl_MyBdocApp.signerRoles.str_role;
				cout << endl << "Additional role: ";
				cin >> cl_MyBdocApp.signerRoles.str_additionalRole;
				
				cl_MyBdocApp.signFile();
				a = 0;
				
				//printf("type:\n     \"s\" - for signing\n     \"o\" - for opening container\n     \"e\" - for exit program\n");
				break;
			
			case 'o': 
				cl_MyBdocApp.initData();
				cl_MyBdocApp.openCont();
				a = 0;
				
				//printf("type:\n     \"s\" - for signing\n     \"o\" - for opening container\n     \"e\" - for exit program\n");
				break;

			default:
				//printf("type:\n     \"s\" - for signing\n     \"o\" - for opening container\n     \"e\" - for exit program\n");
				break;
		}
		printf("type:\n     \"i\" - for signing\n     \"s\" - for signing\n     \"o\" - for opening container\n     \"e\" - for exit program\n");
			
	}while(a!='e');

	return 0;
}
*///--------------------------------------------



