#include <iostream>
#include <string>
#include "smartcardpp.h"

void validateOnPinpad() {
	SmartCardManager mgr;
	uint rdr, i, nr;
	EstEidCard eid(mgr);

	// Find first card
	nr = mgr.getReaderCount();
	for(i = 0; i < nr; i++) {
		if(eid.isInReader(i)) {
			rdr = i;
			break;
		}
	}
	if(i >= nr) {
		std::cout << "No EstEid in reader" << std::endl;
		return;
	}

	eid.connect((uint)rdr);

	// Make sure it's in pinpad enabled reader
	if(!eid.hasSecurePinEntry()) {
		std::cout << "Reader #" << rdr << " has no pinpad" << std::endl;
		return;
	}

	std::cout << "Doing pin validation test on reader #" << rdr << std::endl;
	
	//Transaction _m(m, c);
	//eid.enterPin(EstEidCard::PIN_AUTH, PinString(""));

	byte tries;
	try {
		eid.validateAuthPin(PinString(""), tries);
		std::cout << "PIN OK" << std::endl;
	} catch(std::runtime_error &e) {
		std::cout << "Error: " << e.what() << std::endl;
		std::cout << "Tries left: " << (int)(tries-1) << std::endl;
	}
}

void readEstEidCard(ManagerInterface &m, ConnectionBase *c) {
	EstEidCard eid(m, c);
	std::cout << "\t" << "EstEID: " << eid.readCardName();
	std::cout << " " << eid.readCardID();
	std::cout << " " << eid.readDocumentID();
}

int testCardReading(ManagerInterface &mgr) {
	EstEidCard eidCard(mgr);
	uint nr = mgr.getReaderCount();
	int rv = -1;
	std::cout << "Found " << nr << " readers" << std::endl;
	std::cout << "Nr.\tState\t\tName" << std::endl;
	for(uint i = 0; i < nr; i++) {
		std::string state = mgr.getReaderState(i);

		std::cout << i << "\t" << state;
		std::cout << ( (state.length() < 8) ? "\t\t" : "\t" );
		std::cout << mgr.getReaderName(i);

		if (state.find("PRESENT") != std::string::npos) {
			std::cout << std::endl << "\tATR: " << mgr.getATRHex(i);
			try {
				ConnectionBase *c = mgr.connect(i, false);
				std::cout << " PinPad: " << c->isSecure();
				if(eidCard.isInReader(i)) {
					if(rv == -1) rv = i;
					std::cout << std::endl;
					readEstEidCard(mgr, c);
				}
			}
			catch(std::runtime_error &e) {
				std::cout << "Error: ";
				std::cout << e.what();
			}
		}
		std::cout << std::endl;
	}
	return rv;
}

PinString ReadPinFromStdin() {
	// FIXME: implement
	throw std::runtime_error("Pin entry from keyboard not implemented yet!");
	return PinString("0090");
}

void testKey(ManagerInterface &m, unsigned int reader, EstEidCard::KeyType key) {
	EstEidCard eid(m, reader);
	byte *tmp = (byte *)"12345678901234567890";
	ByteVec dummyHash(tmp, tmp + 20);
	ByteVec cert;
	PinString pin;

	std::cout << "Reading "
		  << ((key == EstEidCard::AUTH) ? "authentication" : "signing")
		  << " certificate ... " << std::flush;

	if(key == EstEidCard::AUTH)
		cert = eid.getAuthCert();
	else
		cert = eid.getSignCert();

	std::cout << "cert is " << cert.size() << " bytes long" << std::endl;

	if(eid.hasSecurePinEntry()) {
		std::cout << "Testing key please enter PIN from PinPad " << std::flush;
		pin = PinString("");
	} else {
		std::cout << "Testing key please enter PIN: " << std::flush;
		pin = ReadPinFromStdin();
	}

	try {
		eid.calcSignSHA1(dummyHash, key, pin);
		std::cout << std::endl;
		std::cout << "Pin OK";
		std::cout << std::endl;
	} catch(AuthError &e) {
		std::cout << std::endl;
		std::cout << "Invalid PIN." << e.what() << std::endl;
	} catch(std::runtime_error &e) {
		std::cout << std::endl;
		std::cout << "Other Error: " << e.what() << std::endl;
	}
}

void testAuth(ManagerInterface &m, unsigned int reader) {
	testKey(m, reader, EstEidCard::AUTH);
}

void testSign(ManagerInterface &m, unsigned int reader) {
	testKey(m, reader, EstEidCard::SIGN);
}

int main(int argc, char *argvp[]) {
	int idx = 0;
	bool testkeys = false;

	if(argc >= 2) {
		std::string arg1(argvp[1]);
		if(arg1 == "--pinpad") {
			validateOnPinpad();
			return 0;
		}
		if(arg1 == "--keys") {
			testkeys = true;
		}
	}
	SmartCardManager mgr;

	idx = testCardReading(mgr);

	if(testkeys && idx >= 0) {
		std::cout << std::endl << "Testing crypto operations with card in reader #" << idx << std::endl;
		testAuth(mgr, idx);
		testSign(mgr, idx);
	}
}
