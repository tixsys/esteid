#include "EstEIDServiceBase.h"
#include "converters.h"

void EstEIDServiceBase::FindEstEID(vector <readerID> & readers) {
    readers.clear();
    Poll();

    for (readerID i = 0; i < cardPresent.size(); i++ )
        if(cardPresent[i]) readers.push_back(i);
}

readerID EstEIDServiceBase::findFirstEstEID() {
    vector <readerID> readers;
    FindEstEID(readers);

    // FIXME: Define a more sane exception to throw from here
    if(readers.size() <= 0)
        throw std::runtime_error("No cards found");
    else
    	return readers[0];
}

void EstEIDServiceBase::Worker() {
	Poll();
}

void EstEIDServiceBase::Poll() {
	try {
		int nReaders;

		{	idAutoLock lock(this);
			nReaders = mgr.getReaderCount();
		}

		EstEidCard card(mgr);
		if(cardPresent.size() != nReaders) {
			cardPresent.resize(nReaders);
			PostMessage(MSG_READERS_CHANGED, nReaders);
		}
		_Poll(card);
	}
	catch(std::runtime_error err) {
		// FIXME: We should throw when called from FindEstEID
		PostMessage(MSG_CARD_ERROR, 0, err.what());
		return;
	}
}

void EstEIDServiceBase::_Poll(EstEidCard & card) {
	for (unsigned int i = 0; i < cardPresent.size(); i++ ) {
		try {
			bool inReader;

			{	idAutoLock lock(this);
				inReader = card.isInReader(i);
			}

			if (inReader && !cardPresent[i]) {
				cardPresent[i] = true;
				PostMessage(MSG_CARD_INSERTED, i);
			}
			else if (!inReader && cardPresent[i]) {
				cardPresent[i] = false;
				PostMessage(MSG_CARD_REMOVED, i);
			}
		}
		catch(std::runtime_error err) {
			PostMessage(MSG_CARD_ERROR, i, err.what());
		}
	}
}

void EstEIDServiceBase::readPersonalData(vector <std::string> & data) {
	readPersonalData(data, findFirstEstEID());
}

void EstEIDServiceBase::readPersonalData(vector <std::string> & data,
                                           readerID reader) {
	idAutoLock lock(this);

    EstEidCard card(mgr, reader);
    card.readPersonalData(data, PDATA_MIN, PDATA_MAX);
}

#define ESTEIDSERVICEBASE_GETCERTIMPL(id) \
	ByteVec EstEIDServiceBase::get##id##Cert() { \
        return get##id##Cert(findFirstEstEID()); \
    }\
    \
	ByteVec EstEIDServiceBase::get##id##Cert(readerID reader) { \
	    idAutoLock lock(this); \
        EstEidCard card(mgr, reader); \
        return card.get##id##Cert(); \
    }

ESTEIDSERVICEBASE_GETCERTIMPL(Auth)
ESTEIDSERVICEBASE_GETCERTIMPL(Sign)

std::string EstEIDServiceBase::signSHA1(std::string hash,
		    EstEidCard::KeyType keyId, std::string pin) {
	return signSHA1(hash, keyId, pin, findFirstEstEID());
}

std::string EstEIDServiceBase::signSHA1(std::string hash,
		    EstEidCard::KeyType keyId, std::string pin, readerID reader) {
	idAutoLock lock(this);

	ByteVec bhash = fromHex(hash);
	if (bhash.size() != 20) {
		throw std::runtime_error("Invalid SHA1 hash");
	}

    EstEidCard card(mgr, reader);

    return toHex(card.calcSignSHA1(bhash, keyId, pin));
}

bool EstEIDServiceBase::getRetryCounts(byte &puk,
		byte &pinAuth,byte &pinSign) {
	return getRetryCounts(puk, pinAuth, pinSign, findFirstEstEID());
}
bool EstEIDServiceBase::getRetryCounts(byte &puk,
		byte &pinAuth,byte &pinSign, readerID reader) {

	idAutoLock lock(this);

    EstEidCard card(mgr, reader);
	return card.getRetryCounts(puk, pinAuth, pinSign);
}
