/*!
	\file		EstEidCardMaintainer.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author$
	\date		$Date$
*/
// Revision $Revision$
#pragma once
#include "EstEidCard.h"

/// Estonian ID card maintaier class, for generating keys and replacing certificates
class EstEidCardMaintainer {
	EstEidCard card;
public:
	EstEidCardMaintainer(EstEidCard &ref) : card(ref) {}
	~EstEidCardMaintainer() {}
	/// generate a new key pair
	void performGenerateNewKeys();
	};
