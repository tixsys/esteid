#include "MyBdocBridge.h"

/*/**************** GURU Antix typed this with hisfingertips **************

class My1EstEIDSigner : // public digidoc::EstEIDSigner,
                        public MyBdocBridge
{
	public:
		My1EstEIDSigner() throw(digidoc::SignException);
		//virtual ~My1EstEIDSigner();
		//digidoc::X509 *cardSignCert;
		//std::string pin;
                void sammkampunn();
		
	//protected:
		//virtual std::string getPin( digidoc::PKCS11Signer::PKCS11Cert certificate ) throw(digidoc::SignException);
		
};
//*************************************************************************/

class My1EstEIDSigner: public digidoc::EstEIDSigner,
		       public MyBdocBridge
{
	public:
		My1EstEIDSigner() throw(digidoc::SignException);
		virtual ~My1EstEIDSigner();
		//X509 *cardSignCert;
		std::string pin;
		void sammkampunn();
		
		
	protected:
		virtual std::string getPin( digidoc::PKCS11Signer::PKCS11Cert certificate ) throw(digidoc::SignException);
		My1EstEIDSigner *m_signer;
};

