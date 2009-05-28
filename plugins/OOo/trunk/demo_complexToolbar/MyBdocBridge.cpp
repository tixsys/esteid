//================================
//
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



//===========================================================
MyBdocBridge::MyBdocBridge() {
}

//===========================================================
//bridge to bdoc functions
void MyBdocBridge::teemingilollus1() {
	((My1EstEIDSigner *)this)->sammkampunn();
//init--------------	
	digidoc::BDoc *locBdoc;
	bool validateOnline;
	digidoc::Signature::Type profile;
	
	((My1EstEIDSigner *)this)->pin = "01497";

	char *val = getenv( "BDOCLIB_CONF_XML" );
	if( val == 0 )
	{
		std::string conf = "BDOCLIB_CONF_XML=" BDOCLIB_CONF_PATH;//

		putenv((char*)&conf );
		val = getenv( "BDOCLIB_CONF_XML" );		
	}

	digidoc::initialize();
	profile = digidoc::Signature::TM;//BES
	validateOnline = true;
	locBdoc = 0;
//--------------

	digidoc::DirectoryX509CertStore::X509CertStore::init( new digidoc::DirectoryX509CertStore() );
//		cout << m_signer.cardSignCert->name<<endl;
//		getchar();
	
	locBdoc = new digidoc::BDoc();
	locBdoc->addDocument(Document("/home/mark/Desktop/Juhan.txt", "file"));

	// Add signature production place.
	digidoc::Signer::SignatureProductionPlace spp("Sittlinn", "Kannimaa", "66 6666", "Tyrrlandia");
	((My1EstEIDSigner *)this)->setSignatureProductionPlace(spp);

	// Add signer role(s).
	digidoc::Signer::SignerRole role("nühkija");
	// Add additional roles.
	role.claimedRoles.push_back("Peller");
	((My1EstEIDSigner *)this)->setSignerRole(role);

	// Sign the BDOC container.
	locBdoc->sign(((My1EstEIDSigner *)this), profile);
	
	// Save the BDOC container.
	std::auto_ptr<digidoc::ISerialize> serializer(new digidoc::ZipSerialize("/home/mark/Desktop/Juhan.bdoc"));
	locBdoc->saveTo(serializer);

	// Destroy certificate store.
	digidoc::DirectoryX509CertStore::X509CertStore::destroy();
}

//===========================================================
//Amazing rabbit hole to the great world of digidoc
MyBdocBridge *MyBdocBridge::getInstance() {
	return (MyBdocBridge *)new My1EstEIDSigner();
}

//===========================================================
My1EstEIDSigner::My1EstEIDSigner() throw(digidoc::SignException)
 :	digidoc::EstEIDSigner( digidoc::Conf::getInstance()->getPKCS11DriverPath() )
{	
	//cardSignCert = NULL;
	try	
	{
		digidoc::PKCS11Signer::getCert();	
	}
	catch( const digidoc::Exception & ) {}	
}

//===========================================================
My1EstEIDSigner::~My1EstEIDSigner()
{}

//===========================================================
std::string My1EstEIDSigner::getPin( digidoc::PKCS11Signer::PKCS11Cert certificate ) throw(digidoc::SignException)
{	
/*	std::string pin ="";
	cout << endl << "Enter PIN2: ";
	for(int u=0; u<5; u++)
	{
		pin += getchar();
		cout << "*";
		
	}
*/	
	return My1EstEIDSigner::pin;
}

//===========================================================
void My1EstEIDSigner::sammkampunn() {
	printf("Samm kampunn huijann\n");

}


