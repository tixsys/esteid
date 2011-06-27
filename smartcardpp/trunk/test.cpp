#include <iostream>
#include <string>
#include <stdlib.h>
#include "smartcardpp.h"
#include "compat_getopt.h"
#include "compat_getpass.h"

void validateOnPinpad() {
	PCSCManager mgr;
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

void readEstEidCard(EstEidCard &eid) {
	std::cout << "\t" << "EstEID: " << eid.readCardName();
	std::cout << " " << eid.readCardID();
	std::cout << " " << eid.readDocumentID();
}

int testCardReading(PCSCManager &mgr) {
	uint nr = mgr.getReaderCount();
	int rv = -1;
	std::cout << "Found " << nr << " readers" << std::endl;
	if (nr == 0)
		exit(1);
	std::cout << "Nr.\tState\t\tName" << std::endl;
	for(uint i = 0; i < nr; i++) {
		std::string state = mgr.getReaderState(i);

		std::cout << i << "\t" << state;
		std::cout << ( (state.length() < 8) ? "\t\t" : "\t" );
		std::cout << mgr.getReaderName(i);

		if (state.find("PRESENT") != std::string::npos) {
			std::cout << std::endl << "\tATR: " << mgr.getATRHex(i);
			try {
				ConnectionBase *c = mgr.connect(i);
				std::cout << " PinPad: " << c->isSecure();
				
				EstEidCard eidCard(mgr, c);
				
				if(eidCard.isInReader(i)) {
					if (rv == -1)
						rv = i;
					std::cout << std::endl;
					readEstEidCard(eidCard);
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
	std::cout << std::endl;
	return PinString(getpass("Enter PIN!"));
}

void testKey(PCSCManager &m, unsigned int reader, EstEidCard::KeyType key) {
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

void testAuth(PCSCManager &m, unsigned int reader) {
	testKey(m, reader, EstEidCard::AUTH);
}

void testSign(PCSCManager &m, unsigned int reader) {
	testKey(m, reader, EstEidCard::SIGN);
}

int main(int argc, char *argvp[]) {
	int idx = 0;
	int opt;
	int longind = 0;
	const char *shortopts = "pkd";
	bool testkeys = false;
	bool debug = false;

	PCSCManager mgr;

	struct option longopts[] = {
		/* name,    has_arg,      flag, val */ /* longind */
		{ "pinpad", no_argument,  0,    'p'  }, /*       0 */
		{ "keys",   no_argument,  0,    'k'  }, /*       1 */
		{ "debug",  no_argument,  0,    'd'  }, /*       1 */
		/* end-of-list marker */
		{ 0, 0, 0, 0 }
	};

	while ((opt = getopt_long_only(argc, argvp, shortopts, longopts, &longind)) != -1) {
		switch (opt) {
			case 'p': // pinpad
				validateOnPinpad();
				return 0;
			case 'k': // keys
				testkeys = true;
				break;
			case 'd':
				mgr.setLogging(&std::cerr);
				break;
			default: /* something unexpected has happened */
				std::cerr << "getopt_long_only returned an unexpected value " << opt << std::endl;
				return 1;
		}
	}

	idx = testCardReading(mgr);

	if(testkeys && idx >= 0) {
		EstEidCard eid(mgr, idx);
		eid.isInReader(idx);
		std::cout << "Card version: " << eid.getCardVersion() << std::endl;
		std::cout << "Card key size: " << eid.getKeySize() << " bits" << std::endl;

		std::cout << std::endl << "Testing crypto operations with card in reader #" << idx << std::endl;
		testAuth(mgr, idx);
		testSign(mgr, idx);

	}
}
