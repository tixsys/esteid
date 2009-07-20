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
namespace digidoc
{
	class MyRealEstEIDSigner: public EstEIDSigner
	{
		public:
			MyRealEstEIDSigner() throw(SignException);
			virtual ~MyRealEstEIDSigner();
			X509 *cardSignCert;
			std::string pin;
			int i_ret;
		
		protected:
			virtual std::string getPin( PKCS11Cert certificate ) throw(SignException);
		
	};

}


class My1EstEIDSigner: //public digidoc::EstEIDSigner,
		       public MyBdocBridge
{
	public:
		My1EstEIDSigner();
		virtual ~My1EstEIDSigner();

		int initData ();
		void Terminate ();
		int signFile ();
		int openCont ();
		int checkCert ();
		void getExceptions(const digidoc::Exception e);
	
		std::string str_pin;
		std::string str_signTime;
		std::string str_signCert;
		std::string str_filepath;
		std::string str_bdocpath;
		int i_signatureCounter;
		

		struct SignaturePlace
		{
			std::string str_city;
			std::string str_stateOrProvince;
			std::string str_postalCode;
			std::string str_countryName;
		} signPlace;
	
		struct Roles
		{
			std::string str_role;
			std::string str_additionalRole;
		} signerRoles;

	
	

	private:
		bool compIDnumber(std::string str_idNum);
		digidoc::X509Cert *singnCert;

		digidoc::Signature::Type profile;
		digidoc::BDoc *locBdoc;		
		bool validateOnline;	
		
		
	//protected:		
};

