#include "nsEstEID.h"
#include "nsEstEIDCertificate.h"

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

#include <iostream>

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
	nsCOMPtr<nsIObserverService> obs =
		do_GetService(NS_OBSERVERSERVICE_CONTRACTID);
    if(!obs)
    	return NS_ERROR_UNEXPECTED;

	nsresult rv = obs->AddObserver(this,
			NS_LITERAL_CSTRING("esteid-card-changed").get(), PR_FALSE);
    if(NS_FAILED(rv))
    	return rv;

    try {
    	service = nsEstEIDService::getInstance();
    } catch(std::runtime_error e) {
    	ESTEID_ERROR("Card Error %s\n", e.what());
    	return NS_ERROR_UNEXPECTED;
    }
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
	for(lt = 0; lt < sizeof(ListenerNames); lt++)
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
	for(lt = 0; lt < sizeof(ListenerNames); lt++)
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
	if(!aHash || !url)
		ESTEID_ERROR_INVALID_ARG;

	if(PL_strlen(aHash) != 40) {
		ESTEID_ERROR("Invalid SHA1 Hash length\n");
		return NS_ERROR_INVALID_ARG;
	}

	nsCOMPtr<nsIWindowWatcher> ww =
		do_GetService(NS_WINDOWWATCHER_CONTRACTID);
	if(!ww) return NS_ERROR_FAILURE;
	ESTEID_DEBUG("AskPINFF: got access to WindowWatcher\n");

	nsCOMPtr<nsIPrompt> prompter;
	ww->GetNewPrompter(0, getter_AddRefs(prompter));
	if(!prompter) return NS_ERROR_FAILURE;
	ESTEID_DEBUG("AskPINFF: prompter activated\n");

	nsCString warn;

	while(true) {
		nsresult rv;
		PRUnichar *pass = nsnull;
		PRBool ok;
		nsCString tmp1, tmp2;
		tmp1 += warn;
		tmp1 += "Dokumendi Allkirjastamine\n";
		rv = GetFirstName(tmp2);
		if(NS_FAILED(rv)) return NS_ERROR_FAILURE;
		tmp1 += tmp2;
		rv = GetLastName(tmp2);
		if(NS_FAILED(rv)) return NS_ERROR_FAILURE;
		tmp1 += " "; tmp1 += tmp2; tmp1 += " (PIN2) "; tmp1 += "\n";

		prompter->PromptPassword(nsnull,
				NS_ConvertUTF8toUTF16(tmp1).get(),
				&pass, 0, 0, &ok);

		if(!ok)	return NS_ERROR_ABORT;

		try {
			try {
				std::string hash(aHash);
				std::string pin(NS_LossyConvertUTF16toASCII(pass).get());
				*_retval = PL_strdup(
				    service->signSHA1(hash,EstEidCard::SIGN,pin).c_str());
				return NS_OK;
			}
			catch(AuthError e) {
				byte puk, pin1, pin2;

				ESTEID_ERROR("Sign failure: %s\n", e.what());

				if(e.m_blocked) { // Card locked. Finito!
					ESTEID_ERROR("Card Locked");
					return NS_ERROR_FAILURE;
				}

				service->getRetryCounts(puk, pin1, pin2);
				warn.Assign("!! VALE PIN !!\nKatseid jäänud: ");
				warn.AppendInt(pin2); warn.Append("\n");
			}
		} catch(std::runtime_error e) {
			ESTEID_ERROR("Card Error %s\n", e.what());
			return NS_ERROR_FAILURE;
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

/*
 * Attribute Getters
 */

NS_IMETHODIMP nsEstEID::GetAuthCert(nsIEstEIDCertificate **_retval) {
	return _GetCert(_retval, AUTH);
}
NS_IMETHODIMP nsEstEID::GetSignCert(nsIEstEIDCertificate **_retval) {
	return _GetCert(_retval, SIGN);
}

#define ESTEID_PD_GETTER_IMP(index, attr) \
    NS_IMETHODIMP nsEstEID::Get##attr(nsACString & _retval) { \
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
