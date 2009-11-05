#include "nsEstEID.h"
#include "nsEstEIDCertificate.h"
#include "converters.h" // FIXME: Remove with other "interesting" code

/* Mozilla includes */
#include <nsStringAPI.h>
#include <plstr.h>
#include <prthread.h>
#include <nsMemory.h>
#include <nsIClassInfoImpl.h>
#include <nsServiceManagerUtils.h>
#include <nsCOMPtr.h>
#include <nsIWindowWatcher.h>
#include <nsIPrompt.h>
#include <nsIDOMWindow.h>
#include <nsIDOMDocument.h>
#include <nsIDOMHTMLDocument.h>


#include <stdio.h>
#include <iostream>

/* Private structures */

struct runArgs {
	nsresult *rv;
	const char *aHash;
	char **retval;
	nsEstEID *me;
	volatile bool closeDialog;
};

NS_IMPL_ISUPPORTS3_CI(nsEstEID, nsIEstEID,
					  nsIObserver, nsISupportsWeakReference)

/**
 * Class constructor.
 */
nsEstEID::nsEstEID()
{
	std::cout << "nsEstEID()" << std::endl;
}

/** destructor code */
nsEstEID::~nsEstEID()
{
    std::cout << "~nsEstEID()" << std::endl;
}

/** * Called from factory
 */
nsresult
nsEstEID::Init()
{
	onPage = false;
	nsCOMPtr<nsIObserverService> obs =
		do_GetService(NS_OBSERVERSERVICE_CONTRACTID);
    if(!obs)
    	return NS_ERROR_UNEXPECTED;

	nsresult rv = obs->AddObserver(this,
			NS_LITERAL_CSTRING("esteid-card-changed").get(), PR_TRUE);
    if(NS_FAILED(rv))
    	return rv;

    try {
    	service = nsEstEIDService::getInstance();
    } catch(std::runtime_error &e) {
    	ESTEID_ERROR("Card Error %s\n", e.what());
    	return NS_ERROR_UNEXPECTED;
    }

    eidgui = do_GetService(NS_ESTEIDPRIVATE_CONTRACTID);

    return NS_OK;
}

/*
 * nsIObserver interface
 */
NS_IMETHODIMP nsEstEID::Observe( nsISupports *aSubject,
								 const char *aTopic,
								 const PRUnichar *someData ) {

	if(!PL_strcasecmp(aTopic, "esteid-card-changed")) {
		nsString s1 = NS_LITERAL_STRING("cardInserted");
		nsString s2 = NS_LITERAL_STRING("cardRemoved");
		nsString s3 = NS_LITERAL_STRING("readersChanged");

		if(s1.Equals(someData)) {
			_FireListeners(INSERT_LISTENERS, 0);
		}
		else if(s2.Equals(someData)) {
			// FIXME: Do not clear if it was another card
			_PersonalData.clear();
			for(int i = 0; i < MAX_CERT; i++)
				_CertData[i].clear();
			_FireListeners(REMOVE_LISTENERS, 0);
		}
		else if(s3.Equals(someData)) {
			_FireListeners(READER_LISTENERS, 0);
		}
	}

	return NS_OK;
}
/*
 * Public methods
 */
NS_IMETHODIMP nsEstEID::GetVersion(nsACString & _retval)
{
    _retval.Assign(NS_LITERAL_CSTRING(ESTEID_MOZPLUG_VERSION));
    // PR_Sleep(PR_SecondsToInterval(15));
    return NS_OK;
}

NS_IMETHODIMP nsEstEID::AddEventListener(const char *type,
                                         nsIEstEIDListener *listener)
{
	if(!type || !listener)
		ESTEID_ERROR_INVALID_ARG;

	unsigned int lt;
	for(lt = 0; lt < MAX_LISTENERS; lt++)
		if(ListenerNames[lt].Equals(type)) break;

	if(lt >= MAX_LISTENERS)
		ESTEID_ERROR_INVALID_ARG;

    vector <ListenerPtr> *l = & _Listeners[lt];

    l->push_back(listener);

    ESTEID_OK;
}

NS_IMETHODIMP nsEstEID::RemoveEventListener(const char *type,
                                            nsIEstEIDListener *listener)
{
	if(!type || !listener)
		ESTEID_ERROR_INVALID_ARG;

	unsigned int lt;
	for(lt = 0; lt < MAX_LISTENERS; lt++)
		if(ListenerNames[lt].Equals(type)) break;

	if(lt >= MAX_LISTENERS)
		return NS_ERROR_INVALID_ARG;

    vector <ListenerPtr> *l = & _Listeners[lt];
    vector <ListenerPtr>::iterator i;

    for (i = l->begin(); i != l->end(); i++ )
        if(*i == listener) l->erase(i);

    return NS_OK;
}

NS_IMETHODIMP nsEstEID::Sign(const char *aHash, const char *url, char **_retval) {
	ESTEID_WHITELIST_REQUIRED;

	if(!aHash || !url)
		ESTEID_ERROR_INVALID_ARG;

	if(PL_strlen(aHash) != 40) {
		ESTEID_ERROR("Invalid SHA1 Hash length\n");
		return NS_ERROR_INVALID_ARG;
	}

	if(!eidgui) return NS_ERROR_FAILURE;

	char *pin = nsnull;
	// PRUnichar *pin = nsnull;
	PRBool retrying = false;
	PRInt16 tries = 3;
	while(true) {
		nsresult rv;
		// PRUnichar *pass = nsnull;
		char *pass = nsnull;
		nsString subject;
		bool pinpad = false;

		try {
                	pinpad = service->hasSecurePinEntry();
		}
		catch(std::runtime_error &e) {
			ESTEID_ERROR("Error finding card: %s\n", e.what());
			return NS_ERROR_FAILURE;
		}

		/* Extract subject line from Certificate */
		nsCOMPtr <nsIEstEIDCertificate> ss;
		rv = GetSignCert(getter_AddRefs(ss));
		if(NS_FAILED(rv)) return NS_ERROR_FAILURE;
		rv = ss->GetCN(subject);
		if(NS_FAILED(rv)) return NS_ERROR_FAILURE;

                if(pinpad) {
			PRThread *thr;
			struct runArgs ra;

			ESTEID_DEBUG("PinPAD detected");
			eidgui->DebugMessage(NS_LITERAL_STRING("PinPAD detected"));

			// Run signing operation in separate thread

			ra.rv = &rv;
			ra.aHash = aHash;
			ra.retval = _retval;
			ra.me = this;
			ra.closeDialog = true;
			thr = PR_CreateThread ( PR_USER_THREAD, runSign, &ra,
						PR_PRIORITY_NORMAL,
						PR_GLOBAL_THREAD,
						PR_JOINABLE_THREAD, 0);

			if(!thr) {
				ESTEID_ERROR("Unable to create signing thread\n");
				return NS_ERROR_OUT_OF_MEMORY;
			}

			ESTEID_DEBUG("Signing thread created\n");

			// Show prompt. It will be closed by runner
			eidgui->PromptForSignPIN(parentWin, subject,
						 url, aHash, pageURL, 30,
						 retrying, tries, &pass);
			ra.closeDialog = false;

			// Make sure our signing thread is finished
			PR_JoinThread(thr);
                } else {
			eidgui->DebugMessage(NS_LITERAL_STRING("No PinPAD available"));
			eidgui->PromptForSignPIN(parentWin, subject,
						 url, aHash, pageURL, 0,
						 retrying, tries, &pass);
			if(!pass)	return NS_ERROR_ABORT;
			rv = doSign(_retval, aHash, pass);
                }

		retrying = true;

		if(NS_SUCCEEDED(rv)) return NS_OK;
		if(rv == NS_ERROR_INVALID_ARG) {
			byte puk, pin1, pin2;
			service->getRetryCounts(puk, pin1, pin2);
			tries = pin2;
		} else {
			return rv;
		}

	}

    /*
        nsCOMPtr<nsIDOMWindow> active;
        nsCOMPtr<nsIDOMWindow> newWindow;

        ww->GetActiveWindow(getter_AddRefs(active));
        rv = windowWatcher->OpenWindow(active,
                                       url,
                                       "_blank",
                                       "centerscreen,chrome,modal,titlebar",
                                       params,
                                       getter_AddRefs(newWindow));

  nsresult rv;
  nsXPIDLString stringOwner;

  if (!dialogTitle) {
    rv = GetLocaleString("string", getter_Copies(stringOwner));
    if (NS_FAILED(rv)) return NS_ERROR_FAILURE;
    dialogTitle = stringOwner.get();
  }

    */
}

/*
 * Private methods
 */
void nsEstEID::_FireListeners(ListenerType lt, PRUint16 reader) {
    vector <ListenerPtr> *l = & _Listeners[lt];
    vector <ListenerPtr>::iterator i;

    std::cout << "Listeners to fire: " << l->size() << std::endl;

    /* Only send events to whitelisted sites */
    if(!_isWhitelisted()) return;

    for (i = l->begin(); i != l->end(); i++ )
        (*i)->HandleEvent(reader);
}

nsresult nsEstEID::_UpdatePersonalData() {
    if(_PersonalData.size() <= 0) {
    	try {
    		service->readPersonalData(_PersonalData);
    		return NS_OK;
    	}
    	catch(std::runtime_error &err) {
    		std::cout << "ERROR: " << err.what() << std::endl;
    		return NS_ERROR_FAILURE;
    	}
    } else {
    	return NS_OK;
    }
}

nsresult nsEstEID::_UpdateCertData(CertId id) {
	if(_CertData[id].size() <= 0) {
		try {
			if(id == AUTH)
				_CertData[AUTH] = service->getAuthCert();
			else if(id == SIGN)
				_CertData[SIGN] = service->getSignCert();

			return NS_OK;
		}
		catch(std::runtime_error &err) {
			std::cout << "ERROR: " << err.what() << std::endl;
			return NS_ERROR_FAILURE;
		}
	} else {
		return NS_OK;
	}
}

nsresult nsEstEID::_GetCert(nsIEstEIDCertificate **_retval, CertId id) {
	nsresult rv = _UpdateCertData(id);
	    if(NS_FAILED(rv)) return rv;

		nsEstEIDCertificate *cert = new nsEstEIDCertificate();
	    if (!cert) {
	      rv = NS_ERROR_OUT_OF_MEMORY;
	      return rv;
	    }
	    rv = cert->Decode(_CertData[id]);
	    if(!NS_SUCCEEDED(rv)) {
	    	// ERROR: Unable to parse certificate data
	    	return NS_ERROR_FILE_CORRUPTED;
	    }

	    *_retval = cert;
	    NS_ADDREF(*_retval);
	    /*
	    rv = cert->QueryInterface(
	        		NS_GET_IID(nsIEstEIDCertificate), (void**)_retval);

	    if (NS_SUCCEEDED(rv) && *_retval) {
	        NS_ADDREF(*_retval);
	    }
	    NS_RELEASE(cert);
	*/
		return rv;
}

bool nsEstEID::_isWhitelisted() {
	if(!onPage) return true;
	if(!eidgui) return false;

	PRBool wl = false;
	nsresult rv = eidgui->IsWhiteListed(pageURL, &wl);

	if(NS_SUCCEEDED(rv) && wl) {
		return true;
	} else {
		if(!wlNotified && eidgui) {
			ESTEID_DEBUG("Showing Notification from %s\n",
					NS_ConvertUTF16toUTF8(pageURL).get());

			nsString tmp(NS_LITERAL_STRING("Denied access to card from "));
			tmp += pageURL;
			eidgui->LogMessage(tmp);
			wlNotified = true;
			eidgui->ShowNotification(parentWin, pageURL);
		}
		return false;
	}
}

nsresult nsEstEID::doSign(char **_retval, const char *aHash, const char *pass) {
	nsresult rv;
	try {
		std::string rsa;
		std::string hash(aHash);
		//std::string pin(NS_LossyConvertUTF16toASCII(pass).get());
		if(pass) {
			std::string pin(pass);
			rsa = service->signSHA1(hash, EstEidCard::SIGN,pin);
		} else {
			// Stupid PC/SC will block the entire thread that
			// manager connection was created from so
			// we need to create a new connection here.
			//
			// FIXME: This code should live somewhere else
			//        Maybe entire card access should be done
			//        from a dedicated thread.
			SmartCardManager mgr;
			EstEidCard card(mgr, service->findFirstEstEID());
			ByteVec bhash = fromHex(hash);
			rsa = toHex(card.calcSignSHA1(bhash, EstEidCard::SIGN,
						      PinString("")));
		}
		*_retval = PL_strdup(rsa.c_str());
		rv = NS_OK;
	}
	catch(AuthError &e) {
		ESTEID_ERROR("Sign failure: %s\n", e.what());

		if(e.m_blocked) { // Card locked. Finito!
			ESTEID_ERROR("Card Locked\n");
			rv = NS_ERROR_FAILURE;
		}
		else if(e.m_aborted) { // User canceled the operation
			ESTEID_DEBUG("User aborted the operation\n");
			rv = NS_ERROR_ABORT;
		} else {
			rv = NS_ERROR_INVALID_ARG;
		}
	} catch(std::runtime_error &e) {
		ESTEID_ERROR("Card Error %s\n", e.what());
		rv = NS_ERROR_FAILURE;
	}

	return rv;
}

void nsEstEID::runSign(void *arg) {
	struct runArgs *a = (runArgs *)arg;
	const char *aHash = a->aHash;
	*(a->rv) = a->me->doSign(a->retval, aHash);
	if(a->closeDialog) {
		ESTEID_DEBUG("Closing Pin Prompt\n");
		a->me->eidgui->ClosePinPrompt(a->me->parentWin);
	}
	ESTEID_DEBUG("Terminating Signing Thread\n");
	return;
}

/* Extract the page URL from DOMWindow passed to us by plugin code */
NS_IMETHODIMP nsEstEID::InitDOMWindow(nsISupports *win)
{
    ESTEID_DEBUG("InitDOMWindow()\n");

    nsCOMPtr <nsIDOMDocument> dd;
    nsCOMPtr <nsIDOMHTMLDocument> dhd;

    if(!win)
        return NS_ERROR_INVALID_ARG;

    nsresult rv = win->QueryInterface(NS_GET_IID(nsIDOMWindow),
									  getter_AddRefs(parentWin));
    if(!NS_SUCCEEDED(rv))
        return NS_ERROR_INVALID_ARG;

    rv = parentWin->GetDocument(getter_AddRefs(dd));
    if(!NS_SUCCEEDED(rv))
        return NS_ERROR_INVALID_ARG;

    rv = dd->QueryInterface(NS_GET_IID(nsIDOMHTMLDocument),
							getter_AddRefs(dhd));
    if(!NS_SUCCEEDED(rv))
        return NS_ERROR_INVALID_ARG;

    rv = dhd->GetURL(pageURL);
    if(!NS_SUCCEEDED(rv))
        return NS_ERROR_INVALID_ARG;

    ESTEID_DEBUG("InitDOMWindow: page URL is %s\n",
                       NS_ConvertUTF16toUTF8(pageURL).get());

    onPage = true;

    // Display a notification if not whitelisted
    _isWhitelisted();

    return NS_OK;
}

NS_IMETHODIMP nsEstEID::KillListeners() {
	ESTEID_DEBUG("KillListeners\n");

	for(unsigned int lt = 0; lt < MAX_LISTENERS; lt++) {
		vector <ListenerPtr> *l = & _Listeners[lt];
		l->clear();
	}

	nsCOMPtr<nsIObserverService> obs =
		do_GetService(NS_OBSERVERSERVICE_CONTRACTID);
	if(!obs)
		return NS_ERROR_UNEXPECTED;

	nsresult rv = obs->RemoveObserver(this,
			NS_LITERAL_CSTRING("esteid-card-changed").get());

	return rv;
}

/*
 * Attribute Getters
 */

NS_IMETHODIMP nsEstEID::GetAuthCert(nsIEstEIDCertificate **_retval) {
	ESTEID_WHITELIST_REQUIRED;

	return _GetCert(_retval, AUTH);
}
NS_IMETHODIMP nsEstEID::GetSignCert(nsIEstEIDCertificate **_retval) {
	ESTEID_WHITELIST_REQUIRED;

	return _GetCert(_retval, SIGN);
}

#define ESTEID_PD_GETTER_IMP(index, attr) \
    NS_IMETHODIMP nsEstEID::Get##attr(nsACString & _retval) { \
	    ESTEID_WHITELIST_REQUIRED; \
        nsresult rv = _UpdatePersonalData(); \
        if(NS_FAILED(rv)) return rv; \
        if(_PersonalData.size() <= index) \
            return NS_ERROR_FAILURE; \
        _retval.Assign(_PersonalData[index].c_str()); \
        return NS_OK; \
    }

ESTEID_PD_GETTER_IMP(EstEidCard::SURNAME,         LastName)
ESTEID_PD_GETTER_IMP(EstEidCard::FIRSTNAME,       FirstName);
ESTEID_PD_GETTER_IMP(EstEidCard::MIDDLENAME,      MiddleName);
ESTEID_PD_GETTER_IMP(EstEidCard::SEX,             Sex);
ESTEID_PD_GETTER_IMP(EstEidCard::CITIZEN,         Citizenship);
ESTEID_PD_GETTER_IMP(EstEidCard::BIRTHDATE,       BirthDate);
ESTEID_PD_GETTER_IMP(EstEidCard::ID,              PersonalID);
ESTEID_PD_GETTER_IMP(EstEidCard::DOCUMENTID,      DocumentID);
ESTEID_PD_GETTER_IMP(EstEidCard::EXPIRY,          ExpiryDate);
ESTEID_PD_GETTER_IMP(EstEidCard::BIRTHPLACE,      PlaceOfBirth);
ESTEID_PD_GETTER_IMP(EstEidCard::ISSUEDATE,       IssuedDate);
ESTEID_PD_GETTER_IMP(EstEidCard::RESIDENCEPERMIT, ResidencePermit);
ESTEID_PD_GETTER_IMP(EstEidCard::COMMENT1,        Comment1);
ESTEID_PD_GETTER_IMP(EstEidCard::COMMENT2,        Comment2);
ESTEID_PD_GETTER_IMP(EstEidCard::COMMENT3,        Comment3);
ESTEID_PD_GETTER_IMP(EstEidCard::COMMENT4,        Comment4);
