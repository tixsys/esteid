/************************************************************************
* Created By Mark Erlich, Smartlink OÜ
* 2009
*************************************************************************/
//================================
//
#include "MyDebug.h"
//#include "MyProtocolHandler.h"

#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>
#include <iostream>

#include <digidocpp/BDoc.h>
#include <digidocpp/BDoc.h>
#include <digidocpp/Conf.h>
#include <digidocpp/SignatureBES.h>
#include <digidocpp/crypto/cert/DirectoryX509CertStore.h>
#include <digidocpp/crypto/cert/X509Cert.h>
#include <digidocpp/io/ZipSerialize.h>
#include <digidocpp/crypto/signer/PKCS11Signer.h>
#include <digidocpp/crypto/signer/EstEIDSigner.h>

#ifndef BDOCLIB_CONF_PATH
#define BDOCLIB_CONF_PATH "digidocpp.conf"
#endif

#ifdef _WIN32
#include <windows.h>
#include <Winreg.h>
#endif

#define EST_ID_CARD_PATH "SOFTWARE\\Estonian ID Card\\digidocpp"

#include "My1EstEIDSigner.h"

using namespace digidoc;
using namespace std;

const char* CERT_STATUSES[3] = { "GOOD", "REVOKED", "UNKNOWN" };



//===========================================================
//********Constructor for OO reachable bridge class**********
MyBdocBridge::MyBdocBridge() {

}

//===========================================================
//*************bridge to bdoc functions**********************
void MyBdocBridge::DigiCheckCert()
{
	ret = ((My1EstEIDSigner *)this)->checkCert();
	//pSignName = ((My1EstEIDSigner *)this)->str_signCert.c_str();
}

//===========================================================
//*************bridge to bdoc functions**********************
//-----------------------------------------------------------
void MyBdocBridge::DigiInit()
{
	ret = ((My1EstEIDSigner *)this)->initData();
}

//-----------------------------------------------------------
void MyBdocBridge::Terminate()
{
	((My1EstEIDSigner *)this)->Terminate();
}

//-----------------------------------------------------------
void MyBdocBridge::DigiSign(const char* pPath, const char* pParam, const char* pPin)
{
	string strPath, strParam;
	//strPath = pPath;
	strParam = pParam;

	((My1EstEIDSigner *)this)->str_filepath = "";
	((My1EstEIDSigner *)this)->signPlace.str_city = "";
	((My1EstEIDSigner *)this)->signPlace.str_stateOrProvince = "";
	((My1EstEIDSigner *)this)->signPlace.str_postalCode = "";
	((My1EstEIDSigner *)this)->signPlace.str_countryName = "";
	((My1EstEIDSigner *)this)->signerRoles.str_role = "";
	((My1EstEIDSigner *)this)->signerRoles.str_additionalRole = "";


	((My1EstEIDSigner *)this)->str_filepath = pPath;

	for(size_t j=0, k=0; j<strParam.size(); j++)
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
void MyBdocBridge::DigiOpen(const char* pPath)
{
	((My1EstEIDSigner *)this)->str_bdocpath = pPath;
	ret = ((My1EstEIDSigner *)this)->openCont();
	
	pRetPath = ((My1EstEIDSigner *)this)->str_filepath.c_str();
	pSignName = ((My1EstEIDSigner *)this)->str_signCert.c_str();
	pSignCity = ((My1EstEIDSigner *)this)->signPlace.str_city.c_str();
	pSignState = ((My1EstEIDSigner *)this)->signPlace.str_stateOrProvince.c_str();
	pSignPostal = ((My1EstEIDSigner *)this)->signPlace.str_postalCode.c_str();
	pSignCountry = ((My1EstEIDSigner *)this)->signPlace.str_countryName.c_str();
	pSignRole = ((My1EstEIDSigner *)this)->signerRoles.str_role.c_str();
	pSignAddRole = ((My1EstEIDSigner *)this)->signerRoles.str_additionalRole.c_str();
	pSignTime = ((My1EstEIDSigner *)this)->str_signTime.c_str();
	iSignCnt = ((My1EstEIDSigner *)this)->i_signatureCounter;

	
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
//******** Parse through all exceptions ***********
void My1EstEIDSigner::getExceptions(const digidoc::Exception e) 
{
#ifdef _WIN32
	pcErrMsg = _strdup(e.getMsg().c_str());
#else
	pcErrMsg = strdup(e.getMsg().c_str());
#endif
	if (e.hasCause())
	{
		for (size_t u=0; u<e.getCauses().size(); u++)
		{
			digidoc::Exception e1 = e.getCauses()[u];
#ifdef _WIN32
			eMessages[iCounter].pcEMsg = _strdup(e1.getMsg().c_str());
#else
			eMessages[iCounter].pcEMsg = strdup(e1.getMsg().c_str());
#endif
			PRINT_DEBUG("ErrorMessage: %s",eMessages[iCounter].pcEMsg);

			iCounter ++;

			getExceptions(e1);
		}
	}
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::initData()
{
	int iRet = 0;	
	iCounter = 0;
	//set installed BDoc library path
	char *val = getenv( "BDOCLIB_CONF_XML" );
	if( val == 0 )
	{
		

	#ifdef _WIN32
		string conf = "BDOCLIB_CONF_XML=" BDOCLIB_CONF_PATH;
		HKEY hkey;
		DWORD dwSize;
		TCHAR tcConfPath[1024];
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT(EST_ID_CARD_PATH), 0, KEY_QUERY_VALUE, &hkey)==ERROR_SUCCESS)
		{
			dwSize = 1024 * sizeof(TCHAR);
			RegQueryValueEx(hkey, TEXT("ConfigFile"), NULL, NULL, (LPBYTE)tcConfPath, &dwSize);
			RegCloseKey(hkey);
		}
		conf = "BDOCLIB_CONF_XML=";
		conf += tcConfPath;
		_putenv(conf.c_str());
	#else
		char* conf = "BDOCLIB_CONF_XML=";
		conf += BDOCLIB_CONF_PATH;
		putenv(conf);	
	#endif		
		val = getenv( "BDOCLIB_CONF_XML" );
	}

	try
	{
		digidoc::initialize();
	}
	catch(const digidoc::BDocException& e)
	{
		PRINT_DEBUG("Caught BDocException: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
			iRet = 1;
	}
	catch(const digidoc::IOException& e)
	{
		PRINT_DEBUG("Caught IOException: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
			iRet = 2;
	}
	catch(const digidoc::OCSPException& e)
	{
		PRINT_DEBUG("Caught OCSPException: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
			iRet = 3;
	}
	catch(const digidoc::SignException& e)
	{
		PRINT_DEBUG("Caught SignException: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
			iRet = 4;
	}
	catch(...)
	{
		pcErrMsg = "Caught unknown exception";
		PRINT_DEBUG(pcErrMsg);
		iRet = 10;
	}


	profile = Signature::TM;//BES
	
	//online validation in open container	
	//validateOnline = true;
	validateOnline = false;
	
	locBdoc = 0;
	return iRet; 
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::signFile ()
{
	int i_ok = 0;
	try
	{		
		MyRealEstEIDSigner m_signer;
		
		i_ok =	m_signer.i_ret;	 //--- check this!!!!
		
		m_signer.pin = str_pin;
		// Init certificate store.
		
		digidoc::X509CertStore::init( new DirectoryX509CertStore() );
		
		if (locBdoc)
		{
			digidoc::initialize();
			
			if (!compIDnumber(m_signer.cardSignCert->name))
			{
				i_ok = 100;
			}
		}		
		else //if it's an new container (in openoffice we will have only 1 file per container)
		{
PRINT_DEBUG("File path for container: %s", str_filepath.c_str());
			locBdoc = new BDoc();
			locBdoc->addDocument(Document(str_filepath, "file"));
			str_bdocpath = str_filepath + ".bdoc";
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
			digidoc::X509CertStore::destroy();
		}
		
	}
	catch(const digidoc::OCSPException& e)
	{
		PRINT_DEBUG("Caught OCSPException: %s", e.getMsg().c_str());
	
		getExceptions(e);
		i_ok |= 50;
	}
	catch(const digidoc::BDocException& e)
	{
		PRINT_DEBUG("Caught BDocException: %s", e.getMsg().c_str());

		getExceptions(e);
		i_ok |= 10;
	}
	catch(const digidoc::IOException& e)
	{
		PRINT_DEBUG("Caught IOException: %s", e.getMsg().c_str());

		getExceptions(e);
		i_ok |= 20;
	}	
	catch(const digidoc::SignException& e)
	{
		PRINT_DEBUG("Caught SignException: %s", e.getMsg().c_str());
		
		getExceptions(e);
		i_ok |= 30;
	}
	catch(const digidoc::Exception& e)
	{
		PRINT_DEBUG("Caught Exception: %s", e.getMsg().c_str());
		
		getExceptions(e);
		i_ok |= 40;
	}
	catch(...)
	{
		PRINT_DEBUG("Caught unknown exception");
		
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
	int i_ret = 0;
	
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

		i_signatureCounter = locBdoc->signatureCount();
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
				i_ret += 1;
				signPlace.str_countryName += "-KEHTETU-;";
				//continue;
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
					i_ret += 100;
				}
			}

			// Get signature production place info.
			Signer::SignatureProductionPlace spp = sig->getProductionPlace();
			if(!spp.isEmpty())
			{
				signPlace.str_city += spp.city.c_str();
				signPlace.str_city += "#";
				signPlace.str_stateOrProvince += spp.stateOrProvince.c_str();
				signPlace.str_stateOrProvince += "#";
				signPlace.str_postalCode += spp.postalCode.c_str();
				signPlace.str_postalCode += "#";
				signPlace.str_countryName += spp.countryName.c_str();
				signPlace.str_countryName += "#";
			}
			else
			{
				signPlace.str_city += "#";
				signPlace.str_stateOrProvince += "#";
				signPlace.str_postalCode += "#";
				signPlace.str_countryName += "#";
			}

			// Get signer role info.
			Signer::SignerRole roles = sig->getSignerRole();
			if(!roles.isEmpty())
			{
				signerRoles.str_role += roles.claimedRoles.at(0);
				signerRoles.str_role += "#";
				//some fix, to be a compatibel with digidoc client
				if (roles.claimedRoles.size() == 1)
					signerRoles.str_additionalRole += "#";
				for(size_t nn=1; nn<roles.claimedRoles.size(); nn++)
				{
					signerRoles.str_additionalRole += roles.claimedRoles.at(1);
					signerRoles.str_additionalRole += "#";
				}
			}
			else
			{
				signerRoles.str_role += "#";
				signerRoles.str_additionalRole += "#";
			}

			// Get signing time.
			str_signTime += sig->getSigningTime().c_str();
			str_signTime += "#";

			// Get signer certificate.
			// TODO: method getSigningCertificate() does not work, implement cert printing after it is fixed.
			X509Cert cert = sig->getSigningCertificate();
			string tempname = cert.getSubject();//.data());.c_str();
			for (size_t u=0; u<tempname.size(); u++)
			{
				if (!memcmp(&tempname[u], ",CN=", 4))
				{
					u += 4;
					while ((memcmp(&tempname[u], ",OU=", 4)) && (u<tempname.size()))
					{
						if (!memcmp(&tempname[u], ",serialNumber=", 14))
						{	//fix for digital stamp - different order
							u += 14;
							str_signCert += "\\,";
						}
						str_signCert += tempname[u++];
					}
				}

			}
			str_signCert += "#";
		}

	// Destroy certificate store.
	X509CertStore::destroy();
	}

	catch(const digidoc::BDocException& e)
	{
		PRINT_DEBUG("Caught BDocException: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
		i_ret += 10000;
	}
	catch(const digidoc::IOException& e)
	{
		PRINT_DEBUG("Caught IOException: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
		i_ret += 20000;
	}
	catch(const digidoc::Exception& e)
	{
		PRINT_DEBUG("Caught Exception: %s", e.getMsg().c_str());
		if (e.hasCause())
			for (size_t u=0; u<e.getCauses().size(); u++)
			{
				pcErrMsg = e.getCauses()[u].getMsg().c_str();
				PRINT_DEBUG("ErrMess%s",pcErrMsg);
			}
		i_ret += 30000;
	}
	catch(...)
	{
		PRINT_DEBUG("Caught unknown exception");
		
		i_ret += 90000;
	}

	// Terminate digidoc library.
	//digidoc::terminate(); //<--deletes temporary files

	return i_ret;
}

//===========================================================
//***********************************************************
bool My1EstEIDSigner::compIDnumber(std::string str_idNum)
{
	string str_cerNum, id1, id2;
	int k = 0;
	size_t found;
	const Signature* sign;

	found=str_idNum.find("serialNumber=");
	if (found!=string::npos)
	{
		id1.assign(str_idNum, int(found), 24);
		cout << "id1: " << id1 << endl;
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
			cout << "id2: " << id2 << endl;
		}
		delete singnCert; 
		singnCert = 0;

		if(!id1.compare(id2))
			return false;
	}		
	
	return true;
}

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
	//MyProtocolHandler::getPin2();
	return pin;
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::checkCert ()
{	
	string strRetCert;
	MyRealEstEIDSigner m_signer;

/*	X509Cert xCert(m_signer.cardSignCert);
	strRetCert = xCert.getSubject();
	for (size_t u=0; u<strRetCert.size(); u++)
	{
		if (!memcmp(&strRetCert[u], ",CN=", 4))
		{
			u += 4;
			while (memcmp(&strRetCert[u], ",OU=", 4))
				str_signCert += strRetCert[u++];
		}

	}*/

	if (!m_signer.i_ret)
	{
		X509Cert activeCert(m_signer.cardSignCert);
		if(!activeCert.isValid())
			m_signer.i_ret = 2;
	}

	return m_signer.i_ret;
}
//===========================================================
//***********************************************************
void My1EstEIDSigner::Terminate()
{	
	digidoc::terminate();
}

//===========================================================
//***********************************************************



