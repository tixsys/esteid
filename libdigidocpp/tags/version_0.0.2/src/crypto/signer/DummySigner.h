#ifndef DUMMYSIGNER_H_
#define DUMMYSIGNER_H_

#include "Signer.h"

namespace digidoc
{

/**
 * Dummy signer
 */
class DummySigner : public digidoc::Signer
{
public:
	DummySigner();
	DummySigner(X509* cert);
	virtual ~DummySigner();

	virtual X509* getCert() throw(SignException);



	virtual void sign(const Digest& digest, Signature& signature) throw(SignException)
	{
	}

private:
	X509* publicCert;

};

}//digidoc

#endif /*DUMMYSIGNER_H_*/
