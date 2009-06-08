#include "SignatureMobile.h"

#include "log.h"

using namespace digidoc;

SignatureMobile::SignatureMobile(BDoc& _bdoc)
: SignatureBES(_bdoc)
{}

SignatureMobile::~SignatureMobile()
{}

void SignatureMobile::sign(Signer* signer) throw(SignatureException, SignException)
{
	DEBUG("SignatureMobile::sign()");

	// Sign with BES profile.
	SignatureBES::sign(signer);
	DEBUG("BES signature successful.");

}

OCSP::CertStatus SignatureMobile::validateOnline() const throw(SignatureException)
{
	return OCSP::GOOD;
}
