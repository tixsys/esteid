#include <digidoc/crypto/signer/EstEIDSigner.h>
#include <digidoc/crypto/signer/PKCS11Signer.h>

#include <digidoc/BDoc.h>

namespace digidoc
{
    class MyEstEIDSigner: public EstEIDSigner
	{
		public:
			MyEstEIDSigner() throw(SignException);
			virtual ~MyEstEIDSigner();
			X509 *cardSignCert;
			std::string pin;
			
		protected:
			virtual std::string getPin( PKCS11Cert certificate ) throw(SignException);
			
	};

	class SimpleBdocApp
	{
		public:
			int initData ();
			int signFile ();
			int openCont ();
		
			std::string str_pin;
			std::string str_filepath;
			std::string str_bdocpath;
			std::string str_signTime;

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
			X509Cert *singnCert;
			std::string str_destination;
			std::string str_origin;
			digidoc::Signature::Type profile;
			//MyEstEIDSigner *m_signer;
			BDoc *locBdoc;
		
			bool validateOnline;	
		
	};	
}
	
