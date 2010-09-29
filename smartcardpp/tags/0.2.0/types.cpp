/*!
	\file		types.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-04-13 13:59:50 +0300 (Mon, 13 Apr 2009) $
*/
// Revision $Revision: 246 $

#include "common.h"

std::ostream& operator<<(std::ostream &out,const ByteVec &vec) {
  out << "[ ";
  for(ByteVec::const_iterator i = vec.begin();i!=vec.end();i++) 
    out << std::hex << std::setfill('0') << std::setw(2) << (int)*i << " ";
  out << "]";
  return out;
  }
