#ifndef EstEIDServiceBase_h
#define EstEIDServiceBase_h

#include "cardlib.h"

typedef unsigned int readerID;

class EstEIDServiceBase {
public:
    /**
     * Find readers with valid card
     */
    void FindEstEID(vector <readerID> & readers);

    /**
     * Read personal data file off the first card
     */
    void readPersonalData(vector <std::string> & data);

    /**
     * Read personal data file off the card in specified reader
     */
    void readPersonalData(vector <std::string> & data,
                              unsigned int reader);
protected:
	friend class idAutoLock;

	enum msgType {
		MSG_CARD_INSERTED,
		MSG_CARD_REMOVED,
		MSG_CARD_ERROR };

	/** Must be called from a background Thread once
	 * in every 500ms for card events to work */
	void Worker();

	/** Lock the lock object */
	virtual void Lock()   {};

	/** Unlock the lock object */
	virtual void UnLock() {};

	/** Callback for the watcher thread. Will be called when
	 * a card event (insertion, removal) is detected. */
	virtual void PostMessage(msgType type, readerID reader,
			std::string msg = "") {};

    EstEIDServiceBase() {};
    virtual ~EstEIDServiceBase() {};
private:
    void findEstEID();
    void Poll();
    void _Poll(EstEidCard & card);

    vector <bool> cardPresent;
    PCSCManager mgr;

    /* Singleton instance variable */
    static EstEIDServiceBase* sEstEIDService;
};

/** A private class to do automatic, stack-based
 * locking/unlocking. Based on the same idea
 * as a Mozillas' nsAutoLock.
 *
 * NB! For internal use only, it lacks a number of sanity checks
 *     that nsAutoLock has.
 */
class idAutoLock {
public:
	idAutoLock(EstEIDServiceBase *s):
		service(s), locked(true) { service->Lock(); }
	~idAutoLock() { if(locked) service->UnLock(); }
private:
	EstEIDServiceBase *service;
	bool locked;
};

#endif // EstEIDServiceBase_h
