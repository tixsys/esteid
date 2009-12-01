#ifndef EstEIDServiceBase_h
#define EstEIDServiceBase_h

#include <smartcard++/smartcard++.h>

#define PDATA_MIN EstEidCard::SURNAME
#define PDATA_MAX EstEidCard::COMMENT4

typedef unsigned int readerID;

class EstEIDServiceBase {
public:
    /**
     * Find readers with valid card
     */
    void FindEstEID(vector <readerID> & readers);

    /**
     * Find the reader with first valid card
     */
    readerID findFirstEstEID();

    /**
     * Read personal data file off the first card
     */
    void readPersonalData(vector <std::string> & data);

    /**
     * Read personal data file off the card in specified reader
     */
    void readPersonalData(vector <std::string> & data, readerID);
    /*
     * Read authentication certificate off the first card
     */
    ByteVec getAuthCert();
    /*
     * Read authentication certificate off the card in specified reader
     */
    ByteVec getAuthCert(readerID);
    /*
     * Read signature certificate off the first card
     */
    ByteVec getSignCert();
    /*
     * Read signature certificate off the card in specified reader
     */
    ByteVec getSignCert(readerID);

    bool getRetryCounts(byte &puk, byte &pinAuth,byte &pinSign);
    bool getRetryCounts(byte &puk, byte &pinAuth,byte &pinSign, readerID);

    bool hasSecurePinEntry();
    bool hasSecurePinEntry(readerID);

    std::string signSHA1(std::string hash, EstEidCard::KeyType keyId,
    		std::string pin);
    std::string signSHA1(std::string hash, EstEidCard::KeyType keyId,
    		std::string pin, readerID);


protected:
	friend class idAutoLock;

	enum msgType {
		MSG_CARD_INSERTED,
		MSG_CARD_REMOVED,
		MSG_READERS_CHANGED,
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
    SmartCardManager mgr;

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
