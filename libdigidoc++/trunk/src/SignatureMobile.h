#pragma once

#include "SignatureBES.h"

namespace digidoc
{

class SignatureMobile: public digidoc::SignatureBES
{
public:
	SignatureMobile( const std::string &path, BDoc& _bdoc );
	virtual ~SignatureMobile();
	virtual OCSP::CertStatus validateOnline() const throw(SignatureException);

protected:
	virtual void sign(Signer* signer) throw(SignatureException, SignException) {};
};

}
