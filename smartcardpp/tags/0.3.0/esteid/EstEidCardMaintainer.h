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
