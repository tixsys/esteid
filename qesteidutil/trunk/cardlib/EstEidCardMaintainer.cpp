/*!
	\file		EstEidCardMaintainer.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-17 16:00:47 +0300 (R, 17 apr 2009) $
*/
// Revision $Revision: 248 $
#include "precompiled.h"
#include "EstEidCardMaintainer.h"

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
