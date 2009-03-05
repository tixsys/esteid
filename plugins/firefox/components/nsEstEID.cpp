#include "nsEstEID.h"

/* Mozilla includes */
#include <nsStringAPI.h>
#include <plstr.h>
#include <prthread.h>
#include <nsMemory.h>
#include <nsIClassInfoImpl.h>
#include <nsServiceManagerUtils.h>
#include <nsCOMPtr.h>

#include <iostream>

NS_IMPL_ISUPPORTS3_CI(nsEstEID, nsIEstEID,
					  nsIObserver, nsISupportsWeakReference)

/* Zero out our singleton instance variable */
nsEstEID* nsEstEID::sEstEID = nsnull;

/**
 * Class constructor.
 * Right now we are making this a singleton.
 * It's generally a good idea to have only a single connection
 * to system's SmartCard manager service.
 * We might reconsider when times change :P
 */
nsEstEID::nsEstEID()
{
	std::cout << "nsEstEID()" << std::endl;

    //NS_ASSERTION(!sEstEID, ">1 EstEID object created");
    //sEstEID = this;
}

/**
 * Singleton "constructor". Will return an existing instance
 * or create a new one if none exists.
 */
nsEstEID* nsEstEID::getInstance() {
    nsEstEID *inst = nsnull;
    if(!sEstEID) {
        nsEstEID* ni = new nsEstEID();
        if (!ni)
            return nsnull;
        NS_ASSERTION(sEstEID, "should be non-null");
        nsresult rv = ni->Init();
        if (NS_FAILED(rv)) {
            delete ni;
            return nsnull;
        }
    }

    NS_ADDREF(inst = sEstEID);
    return inst;
}

/** destructor code */
nsEstEID::~nsEstEID()
{
    sEstEID = nsnull;
    std::cout << "~nsEstEID()" << std::endl;
}

/**
 * Called whenever :P
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
    } catch(std::runtime_error err) {
    	std::cout << err.what() << std::endl;
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
		nsString s1 = NS_LITERAL_STRING("cardInsert");
		nsString s2 = NS_LITERAL_STRING("cardRemove");

		if(s1.Equals(someData)) {
			_FireListeners(INSERT_LISTENERS, 0);
		}
		else if(s2.Equals(someData)) {
			_PersonalData.clear();
			_FireListeners(REMOVE_LISTENERS, 0);
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
	unsigned int lt;
	for(lt = 0; lt < sizeof(ListenerNames); lt++)
		if(ListenerNames[lt].Equals(type)) break;

	if(lt >= MAX_LISTENERS)
		return NS_ERROR_INVALID_ARG;

    vector <ListenerPtr> *l = & _Listeners[lt];

    l->push_back(listener);

    return NS_OK;
}

NS_IMETHODIMP nsEstEID::RemoveEventListener(const char *type,
                                            nsIEstEIDListener *listener)
{
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

/*
 * Attribute Getters
 */
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
