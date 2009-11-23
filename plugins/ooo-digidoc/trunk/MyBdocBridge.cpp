/************************************************************************
* Created By Mark Erlich, Smartlink OÜ
* 2009
*************************************************************************/
//================================
//
#include "MyDebug.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <digidocpp/BDoc.h>
#include <digidocpp/BDoc.h>
#include <digidocpp/Conf.h>
#include <digidocpp/SignatureBES.h>
#include <digidocpp/crypto/cert/DirectoryX509CertStore.h>
#include <digidocpp/crypto/cert/X509Cert.h>
#include <digidocpp/io/ZipSerialize.h>
#include <digidocpp/crypto/signer/PKCS11Signer.h>
#include <digidocpp/crypto/signer/EstEIDSigner.h>
#include <digidocpp/SignatureAttributes.h>
#include <digidocpp/XmlConf.h>

#ifndef BDOCLIB_CONF_PATH
#define BDOCLIB_CONF_PATH "digidocpp.conf"
#endif

#ifdef _WIN32
#include <windows.h>
#include <Winreg.h>
#else
#include <errno.h>
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
	pSerialNr = (char*)((My1EstEIDSigner *)this)->str_signCert.c_str();
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
//void MyBdocBridge::DigiSign(const char* pPath, const char* pParam, const char* pPin)
void MyBdocBridge::DigiSign()
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

	((My1EstEIDSigner *)this)->cpPin = &c_Pin[0];
	iPinReq = 0;
	((My1EstEIDSigner *)this)->ip_pinReq = &iPinReq;

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
		char* conf = "BDOCLIB_CONF_XML=" BDOCLIB_CONF_PATH;
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

		m_signer.pi_pinReq = ip_pinReq;
		
		i_ok =	m_signer.i_ret;	 //--- check this!!!!
		
		//m_signer.pcPin = str_pin.c_str();
		m_signer.pcPin = cpPin;
		
		setLocalConfHack(); // <-- Remove this, when local conf fixed in digidocpp library
		
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
			//Signer::SignatureProductionPlace spp(signPlace.str_city, signPlace.str_stateOrProvince, signPlace.str_postalCode, signPlace.str_countryName);
			digidoc::SignatureProductionPlace spp(signPlace.str_city, signPlace.str_stateOrProvince, signPlace.str_postalCode, signPlace.str_countryName);
       		
			m_signer.setSignatureProductionPlace(spp);

			// Add signer role(s).
			digidoc::SignerRole role(signerRoles.str_role);
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
			digidoc::SignatureProductionPlace spp = sig->getProductionPlace();
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
			digidoc::SignerRole roles = sig->getSignerRole();
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

	catch(const digidoc::SignException& e)
	{
		PRINT_DEBUG("Caught SignException: %s", e.getMsg().c_str());
		i_ret = 1;
	}
	catch(...)
	{
		PRINT_DEBUG("Caught unknown exception");
		i_ret = 1;
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
	* pi_pinReq = 100;

	for (int e=0; e<1; e++)// <--while loop did'nt work out
	{//wait until openoffice is ready.
		if (* pi_pinReq > 1)
			e--;
		else if (* pi_pinReq == 1)
			throw SignException( __FILE__, __LINE__, "PIN acquisition canceled." );
	}

	return pcPin;
}

//===========================================================
//***********************************************************
int My1EstEIDSigner::checkCert ()
{	
//	string strRetCert = "";
	MyRealEstEIDSigner m_signer;

	if (!m_signer.i_ret)
	{
		X509Cert activeCert(m_signer.cardSignCert);
		if(!activeCert.isValid())
			m_signer.i_ret = 2;

		else 
		{
			string tempname = activeCert.getSubject();
			/*for (size_t u=0; u<tempname.size(); u++)
			{
				if (!memcmp(&tempname[u], "serialNumber=", 13))
				{
					str_signCert = "";
					u += 13;
					while ( (u<tempname.size()) && (tempname[u] != ',') )
					{
						str_signCert += tempname[u];
						u++;
					}
				}
				
			}*/
			for (size_t u=0; u<tempname.size(); u++)
			{
				if (!memcmp(&tempname[u], "CN=", 3))
				{
					str_signCert = "";
					u += 3;
					while (u<tempname.size())
					{
						while ( (u<tempname.size()) && ((tempname[u] != '\\') || (tempname[u+1] != ',')))
						{							
							str_signCert += tempname[u];
							u++;
							if (tempname[u] == ',')
								u = tempname.size();													
						}
						str_signCert += " \n";
						u += 2;
					}
				}
				
			}
		}
	}
	return m_signer.i_ret;
}
//===========================================================
//***********************************************************
void My1EstEIDSigner::Terminate()
{	//this also deletes all temprorary files
	digidoc::terminate();
}

//===========================================================
//***********************************************************
void MyRealEstEIDSigner::showPinpad()
{
	* pi_pinReq = 200;
	for (int e=0; e<1; e++)// <--while loop did'nt work out
	{//wait until openoffice is ready.
		if (* pi_pinReq)
			e--;
	}
}

//===========================================================
//***********************************************************
void MyRealEstEIDSigner::hidePinpad()
{
}

//===========================================================
//***********************************************************
void My1EstEIDSigner::setLocalConfHack()
{
	const char *P12Path;
	const char *P12Pass;

	//--------------------
	//get P12 cert path and pwtempLine
#ifdef _WIN32	
	HKEY hKey;
	DWORD dwSize;
	TCHAR PKCS12Path[1024];
	TCHAR PKCS12Pass[24];

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\DigiDocLib\\DIGIDOC_PKCS_FILE"), 0, KEY_QUERY_VALUE, &hKey)==ERROR_SUCCESS)
	{
		dwSize = 1024 * sizeof(TCHAR);
		RegQueryValueEx(hKey, TEXT(""), NULL, NULL, (LPBYTE)PKCS12Path, &dwSize);
		RegCloseKey(hKey);

		P12Path = PKCS12Path;
	}
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\DigiDocLib\\DIGIDOC_PKCS_PASSWD"), 0, KEY_QUERY_VALUE, &hKey)==ERROR_SUCCESS)
	{
		dwSize = 1024 * sizeof(TCHAR);
		RegQueryValueEx(hKey, TEXT(""), NULL, NULL, (LPBYTE)PKCS12Pass, &dwSize);
		RegCloseKey(hKey);

		P12Pass = PKCS12Pass;
	}

#else
	#ifdef __APPLE__
		FILE *listFile;
		string line;
				
		listFile = popen("/usr/bin/defaults read com.estonian-id-card Client.pkcs12Cert > /tmp/P12Path.txt","w");
		
		ifstream ifs1("/tmp/P12Path.txt");
		getline(ifs1,line);
		P12Path = line.c_str();
		ifs1.close();		
		pclose(listFile);
		
		listFile = popen("/usr/bin/defaults read com.estonian-id-card Client.pkcs12Pass > /tmp/P12Pass.txt","w");
		
//		if(pclose(listFile))
//			PRINT_DEBUG("error in pclose: %s\n", strerror(errno));
		ifstream ifs2("/tmp/P12Pass.txt");
		getline(ifs2,line);
		P12Pass = line.c_str();
		ifs2.close();
		pclose(listFile);
//		if(ifs2.close())
//			PRINT_DEBUG("error ifstream.close: %s\n", strerror(errno));
		if(unlink("/tmp/P12Path.txt"))
			PRINT_DEBUG("error removing Path file: %s\n", strerror(errno));
		if(unlink("/tmp/P12Pass.txt"))
			PRINT_DEBUG("error removing Pass file: %s\n", strerror(errno));
	#else
		string localConf;
		localConf = getenv("HOME");
		localConf += "/.config/Estonian ID Card.conf";
	
		string line, tempLine;
		string p12PathLine = "pkcs12Cert=";
		string p12PassLine = "pkcs12Pass=";
		ifstream ifs(localConf.c_str());
		while (!ifs.eof())
		{
			getline(ifs,line);
			tempLine = line.substr( 0, p12PathLine.size() );
			if (!strcmp(tempLine.c_str(), p12PathLine.c_str()))
			{
				P12Path = line.substr( p12PathLine.size(), line.size()-p12PathLine.size() ).c_str();
			}
			tempLine = line.substr( 0, p12PassLine.size() );
			if (!strcmp(tempLine.c_str(), p12PassLine.c_str()))
			{
				P12Pass = line.substr( p12PassLine.size(), line.size()-p12PassLine.size() ).c_str();
			}
		}
		ifs.close();
	#endif
#endif
	//------------------
	//set P12 cert path and pw
	digidoc::Conf *i = NULL;
	try { i = digidoc::Conf::getInstance(); }
	catch( const Exception & e) 
	{
		getExceptions(e);
	}
	i->setPKCS12Cert(P12Path);
	i->setPKCS12Pass(P12Pass);
}
