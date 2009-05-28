/*
 * nsEstEIDCertificate.cpp
 */

#include "nsEstEIDCertificate.h"

/* Mozilla includes */
#include <nsStringAPI.h>
#include <nsMemory.h>
#include <nsIClassInfoImpl.h>
#include <nsServiceManagerUtils.h>
#include <nsCOMPtr.h>
#include "nsIX509CertDB.h"
#include "nsIX509CertValidity.h"
#include "plbase64.h"
#include "nsCRT.h"

NS_IMPL_ISUPPORTS2_CI(nsEstEIDCertificate,
		              nsIEstEIDCertificate,
		              nsISecurityCheckedComponent)

nsEstEIDCertificate::nsEstEIDCertificate()
:mCert(nsnull), mBase64(NULL)
{
	ESTEID_DEBUG("nsEstEIDCertificate()");
}

nsEstEIDCertificate::~nsEstEIDCertificate()
{
	ESTEID_DEBUG("~nsEstEIDCertificate()");
}

nsresult nsEstEIDCertificate::Decode(ByteVec der) {
	ESTEID_DEBUG("nsEstEIDCertificate:Decode");
	if(mCert) NS_RELEASE(mCert);
	if(mBase64) PL_strfree(mBase64);

	nsCOMPtr<nsIX509CertDB> cdb =
		do_GetService(NS_X509CERTDB_CONTRACTID);
    if(!cdb)
    	return NS_ERROR_UNEXPECTED;

    ESTEID_DEBUG("Decode: Obtained access to CertDB service\n");

    // FIXME: This hack will break if std::vector begins to use
    //        some other container besides an internal array.
    //        This is not likely going to happen though.
    mBase64 = PL_Base64Encode((char *)&der[0], der.size(), nsnull);
    if(!mBase64)
    	return NS_ERROR_OUT_OF_MEMORY;

    nsresult rv = cdb->ConstructX509FromBase64(mBase64, & mCert);

    if (NS_SUCCEEDED(rv) && mCert) {
    	ESTEID_DEBUG("Certificate decoded successfully\n");
    	return NS_OK;
    } else {
    	return rv;
    }
}

NS_IMETHODIMP nsEstEIDCertificate::GetCN(nsAString & _retval) {
	return mCert->GetCommonName(_retval);
}

NS_IMETHODIMP nsEstEIDCertificate::GetIssuerCN(nsAString & _retval) {
	return mCert->GetIssuerCommonName(_retval);
}

NS_IMETHODIMP nsEstEIDCertificate::GetKeyUsage(nsAString & _retval) {
	PRUint32 tmp;
	return mCert->GetUsagesString(false, &tmp, _retval);
}

NS_IMETHODIMP nsEstEIDCertificate::GetValidFrom(nsAString & _retval) {
	nsIX509CertValidity *tmp = nsnull;
	nsresult rv = mCert->GetValidity(&tmp);
	if(NS_FAILED(rv) || !tmp)
		return rv;

    return tmp->GetNotBeforeLocalTime( _retval );
}

NS_IMETHODIMP nsEstEIDCertificate::GetValidTo(nsAString & _retval) {
	nsIX509CertValidity *tmp = nsnull;
	nsresult rv = mCert->GetValidity(&tmp);
	if(NS_FAILED(rv) || !tmp)
		return rv;

    return tmp->GetNotAfterLocalTime( _retval );
}

NS_IMETHODIMP nsEstEIDCertificate::GetCert(nsACString & _retval) {
	const nsCString s1 = NS_LITERAL_CSTRING("-----BEGIN CERTIFICATE-----\n");
	const nsCString s2 = NS_LITERAL_CSTRING("\n-----END CERTIFICATE-----\n");
	if(mBase64) {
	    _retval.Assign(mBase64, PL_strlen(mBase64));
	    for(unsigned int i = 64; i < _retval.Length(); i += 65)
	    	_retval.Insert('\n', i);
	    _retval.Insert(s1, 0);
	    _retval.Append(s2);

	    return NS_OK;
	} else
		return NS_ERROR_NULL_POINTER;
}

NS_IMETHODIMP nsEstEIDCertificate::GetSerial(nsAString & _retval) {
	return mCert->GetSerialNumber( _retval );
}

/* ISecurityCheckedComponent */
#define ALL_ACCESS \
    static const char access[] = "AllAccess"; \
    *_retval = (char*)nsMemory::Clone(access, sizeof(access)); \
    return NS_OK;
#define SAME_ORIGIN \
    static const char access[] = "SameOrigin"; \
    *_retval = (char*)nsMemory::Clone(access, sizeof(access)); \
    return NS_OK;
#define NO_ACCESS \
	return NS_OK;

NS_IMETHODIMP nsEstEIDCertificate::CanCreateWrapper(const nsIID *iid, char **_retval) {
	ESTEID_DEBUG("nsEstEIDCertificate::CanCreateWrapper called\n");
    ALL_ACCESS
}
NS_IMETHODIMP nsEstEIDCertificate::CanCallMethod(const nsIID *iid, const PRUnichar *methodName, char **_retval) {
	ESTEID_DEBUG("nsEstEIDCertificate::CanCallMethod called\n");
	NO_ACCESS
}
NS_IMETHODIMP nsEstEIDCertificate::CanGetProperty(const nsIID *iid, const PRUnichar *propertyName, char **_retval) {
	ESTEID_DEBUG("nsEstEIDCertificate::CanGetProperty called\n");
	ALL_ACCESS
}
NS_IMETHODIMP nsEstEIDCertificate::CanSetProperty(const nsIID *iid, const PRUnichar *propertyName, char **_retval) {
	ESTEID_DEBUG("nsEstEIDCertificate::CanSetProperty called\n");
	NO_ACCESS
}
