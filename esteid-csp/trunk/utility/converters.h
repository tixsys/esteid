/*!
	\file		converters.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-13 11:34:43 +0300 (Mon, 13 Apr 2009) $
*/
// Revision $Revision: 240 $

std::string toHex(const std::vector<unsigned char> & byteVec);
std::vector<unsigned char> fromHex(const std::string & hexStr);
