//================================
//
#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>
#include <iostream>

#include <digidoc/BDoc.h>
#include <digidoc/BDoc.h>
#include <digidoc/log.h>
#include <digidoc/Conf.h>
#include <digidoc/SignatureBES.h>
#include <digidoc/crypto/cert/DirectoryX509CertStore.h>
#include <digidoc/crypto/cert/X509Cert.h>
#include <digidoc/io/ZipSerialize.h>
#include <digidoc/crypto/signer/PKCS11Signer.h>
#include <digidoc/crypto/signer/EstEIDSigner.h>

#ifndef BDOCLIB_CONF_PATH
#define BDOCLIB_CONF_PATH "bdoclib.conf"
#endif

#include "My1EstEIDSigner.h"
//#include "MyOOoBridge.h"

using namespace digidoc;
using namespace std;

const char* CERT_STATUSES[3] = { "GOOD", "REVOKED", "UNKNOWN" };


//===========================================================
//********Constructor for OO reachable bridge class**********
MyBdocBridge::MyBdocBridge() {
}

//===========================================================
//*************bridge to bdoc functions**********************
void MyBdocBridge::teemingilollus1()
{
	((My1EstEIDSigner *)this)->sammkampunn();	
}

//-----------------------------------------------------------
void MyBdocBridge::DigiInit()
{
	((My1EstEIDSigner *)this)->initData();
}

//-----------------------------------------------------------
void MyBdocBridge::DigiSign(char* pPath, char* pParam, char* pPin)
{
	//((My1EstEIDSigner *)this)->str_filepath = "/home/mark/Desktop/Juhan.txt";
	//((My1EstEIDSigner *)this)->str_filepath = pPath;
	//((My1EstEIDSigner *)this)->signPlace.str_city = "Linn";
	//((My1EstEIDSigner *)this)->signPlace.str_stateOrProvince = "Maakond";
	//((My1EstEIDSigner *)this)->signPlace.str_postalCode = "99999";
	//((My1EstEIDSigner *)this)->signPlace.str_countryName = "Riik";
	//((My1EstEIDSigner *)this)->signerRoles.str_role = "pealik";
	//((My1EstEIDSigner *)this)->signerRoles.str_additionalRole = "orjaosakond";
	//((My1EstEIDSigner *)this)->str_pin = pPin;

	string strPath, strParam;
	strPath = pPath;
	strParam = pParam;

	((My1EstEIDSigner *)this)->str_filepath = "";
	((My1EstEIDSigner *)this)->signPlace.str_city = "";
	((My1EstEIDSigner *)this)->signPlace.str_stateOrProvince = "";
	((My1EstEIDSigner *)this)->signPlace.str_postalCode = "";
	((My1EstEIDSigner *)this)->signPlace.str_countryName = "";
	((My1EstEIDSigner *)this)->signerRoles.str_role = "";
	((My1EstEIDSigner *)this)->signerRoles.str_additionalRole = "";

	for (int i=7; i<strPath.size(); i++)
		((My1EstEIDSigner *)this)->str_filepath += strPath[i];

	for(int j=0, k=0; j<strParam.size(); j++)
	{
		while (strParam[j] != '#')
		{
			switch (k)
			{
				case 0:
					((My1EstEIDSigner *)this)->signPlace.str_city += strParam[j];
					break;
				case 1:
					((My1EstEIDSigner *)this)->signPlace.str_stateOrProvince += strParam[j];
					break;
				case 2:
					((My1EstEIDSigner *)this)->signPlace.str_postalCode += strParam[j];
					break;
				case 3:
					((My1EstEIDSigner *)this)->signPlace.str_countryName += strParam[j];
					break;
				case 4:
					((My1EstEIDSigner *)this)->signerRoles.str_role += strParam[j];
					break;
				case 5:
					((My1EstEIDSigner *)this)->signerRoles.str_additionalRole += strParam[j];
					break;
				default:
					k++;
			}
			
			j++;
		}
		k++;
	}

	((My1EstEIDSigner *)this)->str_pin = pPin;

	ret = ((My1EstEIDSigner *)this)->signFile();

}

//-----------------------------------------------------------
void MyBdocBridge::DigiOpen()
{
	//((My1EstEIDSigner *)this)->str_bdocpath = "/home/mark/Desktop/Juhan.bdoc";
	((My1EstEIDSigner *)this)->openCont();
}


//===========================================================
//*****Amazing rabbit hole to the great world of digidoc******
MyBdocBridge *MyBdocBridge::getInstance() {
	return (MyBdocBridge *)new My1EstEIDSigner();
}

//===========================================================
//*********Constructor for digidoc functional class**********
My1EstEIDSigner::My1EstEIDSigner()
{}

//===========================================================
//**********Destructor for digidoc functional class**********
My1EstEIDSigner::~My1EstEIDSigner()
{}

//===========================================================
//******** Mister Antigo's Special Debug Sentence ***********
void My1EstEIDSigner::sammkampunn() {
	printf("Samm kampunn huijann\n");
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::initData()
{
	//set installed BDoc library path
	char *val = getenv( "BDOCLIB_CONF_XML" );
	if( val == 0 )
	{
		char* conf = "BDOCLIB_CONF_XML=" BDOCLIB_CONF_PATH;//
		putenv(conf);
		val = getenv( "BDOCLIB_CONF_XML" );
	}

	digidoc::initialize();

	profile = Signature::TM;//BES
	//str_destination = str_bdocpath;	
	validateOnline = true;
	
	locBdoc = 0;
	return 0; 
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::signFile ()
{
/*
cout<<"linn: "<<signPlace.str_city<<endl;
cout<<"maak: "<<signPlace.str_stateOrProvince<<endl;
cout<<"post: "<<signPlace.str_postalCode<<endl;
cout<<"riik: "<<signPlace.str_countryName<<endl;
cout<<"amet: "<<signerRoles.str_role<<endl;
cout<<"auaste: "<< signerRoles.str_additionalRole<<endl;
cout <<"PIN: "<<str_pin<<endl;
*/
	int i_ok = 0;
	try
	{		
		MyRealEstEIDSigner m_signer;
		
		i_ok =	m_signer.i_ret;	
		
		m_signer.pin = str_pin;
		// Init certificate store.
		
		digidoc::X509CertStore::init( new DirectoryX509CertStore() );
//		cout << m_signer.cardSignCert->name<<endl;
//		getchar();
		
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
			str_bdocpath = str_filepath + ".bdoc";
			std::auto_ptr<ISerialize> serializer(new ZipSerialize(str_bdocpath));
			locBdoc->saveTo(serializer);

			// Destroy certificate store.
			digidoc::X509CertStore::destroy();
		}
		
	}
	
	catch(const digidoc::BDocException& e)
	{
		ERR("Caught BDocException: %s", e.getMsg().c_str());
		
		i_ok |= 10;
	}
	catch(const digidoc::IOException& e)
	{
		ERR("Caught IOException: %s", e.getMsg().c_str());
		i_ok |= 20;
	}
	catch(const digidoc::SignException& e)
	{
		ERR("Caught SignException: %s", e.getMsg().c_str());
		i_ok |= 30;
	}
	catch(const digidoc::Exception& e)
	{
		ERR("Caught Exception: %s", e.getMsg().c_str());
		i_ok |= 40;
	}
	catch(...)
	{
		ERR("Caught unknown exception");
		i_ok |= 90;
	}

	
	// Terminate digidoc library.
	digidoc::terminate();
	
	// free memory

	delete locBdoc;
	locBdoc = 0;

	return i_ok;
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::openCont ()
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
		//cout << endl << str_origin << endl; 
	
		// Print container signatures list.
		printf("\nSignatures:\n");
		for(unsigned int i = 0; i < locBdoc->signatureCount(); i++)
		{
	    
			// Get signature from bdoc cotainer.
	    		const Signature* sig = locBdoc->getSignature(i);
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
				//If signature is not offline valid, other data fields can not be read from it.
				printf("    Offline validation: FAILED (%s)\n", e.getMsg().c_str());
				continue;
			}

			// Validate signature online. 
			//Checks the validity of the certificate used to sign the bdoc container.
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

				cout << "********************************************"  << endl;
				cout << "*   City:            " << signPlace.str_city << endl;
				cout << "*   State/Province:  " << signPlace.str_stateOrProvince << endl;
				cout << "*   Postal/ZIP:      " << signPlace.str_postalCode << endl;
				cout << "*   Country:         " << signPlace.str_countryName << endl;
				cout << "********************************************"  << endl;
			}

			// Get signer role info.
			Signer::SignerRole roles = sig->getSignerRole();
			if(!roles.isEmpty())
			{
				signerRoles.str_role = roles.claimedRoles.at(0);
				signerRoles.str_additionalRole = roles.claimedRoles.at(1);
				cout << "*   " << signerRoles.str_role << endl;
				cout << "*   " << signerRoles.str_additionalRole << endl;
				cout << "********************************************"  << endl;
			}

			// Get signing time.
			str_signTime = sig->getSigningTime().c_str();
			cout << "*   Signing time: " << str_signTime << endl;
			cout << "********************************************"  << endl;

		// Get signer certificate.
		// TODO: method getSigningCertificate() does not work, implement cert printing after it is fixed.
		X509Cert cert = sig->getSigningCertificate();
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

//===========================================================
//***********************************************************
bool My1EstEIDSigner::compIDnumber(std::string str_idNum)
{}

//===========================================================
//***********Constructor for real digidoc connection*********
MyRealEstEIDSigner::MyRealEstEIDSigner() throw(SignException)
:	EstEIDSigner( Conf::getInstance()->getPKCS11DriverPath() )
{
	cardSignCert = NULL;
	i_ret=0;
	try	
	{
		cardSignCert = getCert();	
	}
	catch( const Exception & ) {
		i_ret=1;
	}	
}

//===========================================================
//***********Destructor for real digidoc connection**********
MyRealEstEIDSigner::~MyRealEstEIDSigner()
{}

//===========================================================
//***********************************************************
std::string MyRealEstEIDSigner::getPin( PKCS11Cert certificate ) throw(SignException)
{	
	//somehow have to fix return, when PIN is wrong
	return pin;
}

//===========================================================
//***********************************************************

//===========================================================
//***********************************************************




