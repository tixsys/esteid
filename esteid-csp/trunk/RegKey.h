/*!
	\file		RegKey.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2008-12-23 19:01:46 +0200 (Tue, 23 Dec 2008) $
*/
// Revision $Revision: 153 $

#pragma once
#include <string>
#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

class RegKey
{
	HKEY key;
public:
	RegKey(HKEY parent,tstring name,REGSAM access = KEY_READ);
	RegKey(HKEY parent,std::string name,REGSAM access = KEY_READ);
	~RegKey(void);
	std::string readString(std::string name);
	tstring RegKey::readString(tstring name);

	void setString(tstring valueName,tstring value);
	void setInt(tstring valueName,DWORD value);
	void setBin(tstring valueName,std::vector<BYTE> &bin);

	void deleteKey(tstring keyName);

	operator HKEY() const {return key;}
};
