/*!
	\file		converters.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-13 11:34:43 +0300 (E, 13 apr   2009) $
*/
// Revision $Revision: 240 $

#include <smartcard++/common.h>
#include <algorithm>

typedef	std::vector<unsigned char> ByteVec;

std::string toHex(const std::vector<unsigned char> &byteVec) {
	std::ostringstream buf;
	for(ByteVec::const_iterator it = byteVec.begin();it!=byteVec.end();it++)
		buf << std::setfill('0') << std::setw(2) <<std::hex <<
		(short) *it;
	return buf.str();
	}

bool getOneChar(ByteVec &hexStr,int &c) {
	if (hexStr.empty()) return false;
	int ch = hexStr.back();
	hexStr.pop_back();
	while (ch == ' ' || ch == '\r' || ch == '\t' || ch == '\n') {
		if (hexStr.empty()) return false;
		ch = hexStr.back();
		hexStr.pop_back();
		}
	if (ch >= '0' && ch <= '9')
		c = ch - '0';
	else if (ch >= 'A' && ch <= 'F')
		c = ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'f')
		c = ch - 'a' + 10;
	return true;
}

ByteVec fromHex(const std::string &hexStr) {
	ByteVec retVal,inpVal(hexStr.length(),'\0');
	copy(hexStr.begin(),hexStr.end(),inpVal.begin());
	reverse(inpVal.begin(),inpVal.end());
	int c1,c2;
	while(getOneChar(inpVal,c1) && getOneChar(inpVal,c2)) {
		retVal.push_back( 	(c1 << 4)  | c2 );
		}
	return retVal;
	}
