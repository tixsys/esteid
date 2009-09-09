/*!
	\file		CSPEestEid.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2008-12-23 19:01:46 +0200 (Tue, 23 Dec 2008) $
*/
// Revision $Revision: 153 $

#pragma once
#include "Csp.h"



class CspEstEid : public Csp
{
	static struct cardSetup {
		const tstring name;
		const tstring atr;
		const tstring atrMask;
	} cards[3];
	void regCard(cardSetup &card);
public:
	virtual CSPContext * createCSPContext();

	virtual HRESULT DllUnregisterServer(void);
	virtual HRESULT DllRegisterServer(void);

	CspEstEid(HMODULE module,TCHAR *);
	~CspEstEid(void);
};
