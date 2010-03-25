/*
 * esteid-csp - CSP for Estonian EID card
 *
 * Copyright (C) 2008-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "precompiled.h"
#include "RegKey.h"

#include <tchar.h>

RegKey::RegKey(HKEY parent,std::string name,REGSAM access)
{
	DWORD dwDisp;
	LONG nStatus = RegCreateKeyExA(parent,name.c_str(),
		0,"",REG_OPTION_NON_VOLATILE,access,
		NULL,&key,
		&dwDisp);
	if (nStatus != ERROR_SUCCESS)
		nStatus = RegOpenKeyExA(parent,
			name.c_str(),0,KEY_READ,&key);
}

RegKey::RegKey(HKEY parent,tstring name,REGSAM access) {
	DWORD dwDisp;
	LONG nStatus= RegCreateKeyEx(parent,name.c_str(),
		0,_TEXT(""),REG_OPTION_NON_VOLATILE,access,
		NULL,&key,
		&dwDisp);
	}

RegKey::~RegKey(void)
{
	RegCloseKey(key);
}

std::string RegKey::readString(std::string name) {
	DWORD dwValSize = 0;
	RegQueryValueExA(key,
			name.c_str(),
			0, 0,NULL,&dwValSize);
	std::vector<unsigned char> buf(dwValSize,'\0');
	RegQueryValueExA(key,
			name.c_str(),
			0, 0,&buf[0],&dwValSize);
	std::string retVal(buf.size()-1,'\0');
	copy(buf.begin(),buf.end()-1,retVal.begin());
	return retVal;
	}

tstring RegKey::readString(tstring name) {
	DWORD dwValSize = 0;
	RegQueryValueEx(key,
			name.c_str(),
			0, 0,NULL,&dwValSize);
	std::vector<wchar_t> buf(dwValSize,'\0');
	RegQueryValueEx(key,
			name.c_str(),
			0, 0,(LPBYTE)&buf[0],&dwValSize);
	tstring retVal(buf.size()-1,'\0');
	copy(buf.begin(),buf.end()-1,retVal.begin());
	return retVal;
	}

void RegKey::setString(tstring valueName,tstring value) {
    LONG nStatus = RegSetValueEx(key,
		valueName.c_str(),0,REG_SZ,
		(LPBYTE)value.c_str(),
		(DWORD)(value.length() + 1) * sizeof(TCHAR));
	}

void RegKey::setInt(tstring valueName,DWORD value) {
    LONG nStatus = RegSetValueEx(key,
		valueName.c_str(),0,
		REG_DWORD,
		(LPBYTE)&value,sizeof(DWORD));
	}

void RegKey::setBin(tstring valueName,std::vector<BYTE> &bin) {
    LONG nStatus = RegSetValueEx(key,
		valueName.c_str(),0,
		REG_BINARY,
		&bin[0],(DWORD)bin.size());
	}

void RegKey::deleteKey(tstring keyName) {
	RegDeleteKey(key,keyName.c_str());
	}


