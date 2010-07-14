#include <smartcardpp/sim/SIMCard.h>
#include "common.h"

SIMCard::SIMCard(ManagerInterface &ref) : CardBase(ref) {}

SIMCard::~SIMCard(void) {}

bool SIMCard::isInReader(unsigned int idx) {
	//select MF, try reading DATAFILE_GSM ?
	return false;
	}

std::string SIMCard::readICC() {
	Transaction _m(mManager,mConnection);
	selectMF(true);
	FCI fileInfo = selectEF(ELEMENTARY_ICC1,true);
	readEF(fileInfo.fileLength);
	return "";
}
