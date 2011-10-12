/*
* SMARTCARDPP
* 
* This software is released under either the GNU Library General Public
* License (see LICENSE.LGPL) or the BSD License (see LICENSE.BSD).
* 
* Note that the only valid version of the LGPL license as far as this
* project is concerned is the original GNU Library General Public License
* Version 2.1, February 1999
*
*/



#include "EstEidCardMaintainer.h"
#include "common.h"

void EstEidCardMaintainer::performGenerateNewKeys() {
	card.readCardID();
	card.selectEF(EstEidCard::FILEID_KEYPOINTER);
	ByteVec keyRec = card.readRecord(1);
	if (keyRec.size() != 0x15)
			throw CardDataError("key ptr len is not 0x15");
	ByteVec authPtr(keyRec.begin() + 0x09, keyRec.begin() + 0x0A);
	ByteVec signPtr(keyRec.begin() + 0x13, keyRec.begin() + 0x14);

	card.selectMF();
	card.setSecEnv(3);
	card.selectDF(EstEidCard::FILEID_APP);
	card.setSecEnv(3);
	/*CardBase::FCI fileInfo = */card.selectEF(0x0013);
	card.readEF(1);
}
