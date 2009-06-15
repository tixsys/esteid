#include "SignatureMobile.h"

#include "log.h"

using namespace digidoc;

SignatureMobile::SignatureMobile(const std::string &p, BDoc& _bdoc)
: SignatureTM(p,_bdoc)
{}

SignatureMobile::~SignatureMobile()
{}

OCSP::CertStatus SignatureMobile::validateOnline() const throw(SignatureException)
{
	return OCSP::GOOD;
}
