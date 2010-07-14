/*!
	\file		EstEidCardMaintainer.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-09 19:31:59 +0300 (Thu, 09 Jul 2009) $
*/
// Revision $Revision: 351 $
#include <smartcardpp/esteid/EstEidCardMaintainer.h>
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
