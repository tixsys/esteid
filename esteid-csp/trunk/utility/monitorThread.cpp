/*!
	\file		monitorThread.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-08-17 15:39:43 +0300 (Mon, 17 Aug 2009) $
*/
// Revision $Revision: 440 $

#include "precompiled.h"
#include "monitorThread.h"
#include <smartcardpp/PCSCManager.h>
#include <smartcardpp/esteid/EstEidCard.h>

#include <iostream>

struct checker {
    monitorObserver &observer;
	PCSCManager mgr;
    EstEidCard card;
	std::vector<bool> cardPresent;
	checker(monitorObserver &ref) : observer(ref),card(mgr) {}
	void executeCheck();
    bool readerHasCard(EstEidCard &card,int i);
    };

bool checker::readerHasCard(EstEidCard &card,int i) {
    std::string state = mgr.getReaderState(i);
    if (state.find("PRESENT") == std::string::npos ) return false;
    if (cardPresent[i]) //caching/optimization
        return true;
    return card.isInReader(i);
    }

void checker::executeCheck() {
    unsigned int readers = mgr.getReaderCount(true);
    if (readers != cardPresent.size()) {
        cardPresent.resize(readers);
        observer.onEvent(READERS_CHANGED,0);
        }
	bool removeFired = false;
	int cardsFound = 0;
    for (size_t i = 0; i < cardPresent.size() ; i++ ) {
		bool haveCard = readerHasCard(card,(int)i);
		if (haveCard) cardsFound++;
        if (haveCard && !cardPresent[i]) {
            cardPresent[i] = true;
            observer.onEvent(CARD_INSERTED,(int)i);
            }
        if (!haveCard && cardPresent[i]) {
            cardPresent[i] = false;
            observer.onEvent(CARD_REMOVED,(int)i);
			removeFired = true;
            }
        }
	if (removeFired && cardsFound == 0) 
		observer.onEvent(NO_CARDS_LEFT,0);
    }

void monitorThread::execute() {
#ifdef _WIN_SERVICE_STARTUP_HACK
//smartcardmanager constructor throws somewhere,
//when dependent services are still starting. need to investigate more
	for(;;) {
		try {
		    checker test_check(observer);
			break;
		} catch(...) {}
		wait(1000);
		}
#endif

//    std::cout << "monitorThread::execute"<<std::endl;
    checker check(observer);
    for(;;) {
		try {
			mutexObjLocker lock_(lock);
			threadObj::wait(500);
			check.executeCheck();
		} catch(std::runtime_error &) {}
        }
    }
