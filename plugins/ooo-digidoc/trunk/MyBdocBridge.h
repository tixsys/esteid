/*
 * ooo-digidoc - OpenOffice.org digital signing extension
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
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

/************************************************************************
* Created By Mark Erlich, Smartlink OÜ
* 2009
*************************************************************************/


// MyDdocBridge.h
// Korralikud HEADERI define-d
// -------------------------------------------------------------------------------------------------
// -- Comments from Antix:									  --
// -- Siin failis EI TOHI OLLA MITTE YHTEGI BDOCi ega TAVALISE STLi andmetüüpi.			  --
// -- Kuna OOo-l on oma samm kampunn STL, siis muidu on kohe symbolite ja prototyypidega ikaldus. --
// -------------------------------------------------------------------------------------------------

class MyBdocBridge {
protected:
	MyBdocBridge();
public:
	static MyBdocBridge * getInstance();
	void DigiSign();
	void DigiOpen(const char* pPath);
	void DigiInit();
	void DigiCheckCert();
	void Terminate();

	int ret;
	int iSignCnt;
	const char* pRetPath;
	const char* pSignName;
	const char* pSignTime;
	const char* pSignCity;
	const char* pSignState;
	const char* pSignPostal;
	const char* pSignCountry;
	const char* pSignRole;
	const char* pSignAddRole;
	const char* pPath;
	const char* pParam;
	char c_Pin[13]; //max length for pin2 is 12
	char* pSerialNr;

	const char* pcErrMsg;
	struct {
		const char* pcEMsg;
	} eMessages [20];

	int iCounter;
	volatile int iPinReq;
    const char * locErrMess;
};
