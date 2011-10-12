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

#include "common.h"
#include "SCError.h"

SCError::SCError(long err) : runtime_error("smart card API error"),error(err)
{
	std::ostringstream buf;
	switch(err & 0xFFFFFFFF) {
		case 0x80100017 : //SCARD_E_READER_UNAVAILABLE
			buf << "No smart card readers"; break;
		case 0x80100069 : //SCARD_W_REMOVED_CARD
			buf << "No card in specified reader"; break;
		case 0x80100011 : //SCARD_E_INVALID_VALUE .. needs trapping
			buf << "Another application is using the card"; break;
		case 0x8010000b : //SCARD_E_SHARING_VIOLATION
			buf << "The smart card cannot be accessed because of other connections outstanding"; break;
		case 0x8010000f : //SCARD_E_PROTO_MISMATCH
			buf << "The requested protocols are incompatible with the protocol currently in use with the smart card"; break;
		case 0x8010001D : // SCARD_E_NO_SERVICE
			buf << "Smart card manager (PC/SC service) is not running"; break;	
		case 0x80100066 : // SCARD_W_UNRESPONSIVE_CARD
			buf << "The card is not responding to reset"; break;
		default:
			buf << "exception:'" << runtime_error::what() << 
				"' code:'0x" <<std::hex << std::setfill('0') <<
				std::setw(8) << error << "'";
		}
	desc = buf.str();
}

void SCError::check(long err)
{
	switch( err )
	{
		//these are not errors
		case 0x80100068: //SCARD_W_RESET_CARD
			return;
	}
	if (err)
		throw SCError(err); 
}
