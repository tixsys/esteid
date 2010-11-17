/*!
	\file		EstEidCardMaintainer.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-07 08:35:50 +0300 (Tue, 07 Jul 2009) $
*/
// Revision $Revision: 346 $
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
