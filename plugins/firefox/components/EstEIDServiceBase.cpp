#include "EstEIDServiceBase.h"

void EstEIDServiceBase::FindEstEID(vector <readerID> & readers) {
    readers.clear();
    Poll();

    for (readerID i = 0; i < cardPresent.size(); i++ )
        if(cardPresent[i]) readers.push_back(i);
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
		cardPresent.resize(nReaders);
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
    vector <readerID> readers;
    FindEstEID(readers);

    // FIXME: Define a more sane exception to throw from here
    if(readers.size() <= 0)
        throw std::runtime_error("No cards found");
    else
        readPersonalData(data, readers[0]);
}

void EstEIDServiceBase::readPersonalData(vector <std::string> & data,
                                           unsigned int reader) {
	idAutoLock lock(this);

    EstEidCard card(mgr, reader);
    card.readPersonalData(data, PDATA_MIN, PDATA_MAX);
}
