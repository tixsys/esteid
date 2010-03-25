/*
 * SmartCardRemoval
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

// SmartCardRemoval.cpp : Implementation of WinMain


#include "precompiled.h"
#include "resource.h"
#include "SmartCardRemoval.h"

#include <fstream>
#include <iostream>
#include <exception>
#include <utility/monitorThread.h>
#include "ProcessStarter.h"
#include "Tlhelp32.h"

#pragma comment(lib,"crypt32")

using std::endl;

class CSmartCardRemovalModule : 
		public CAtlServiceModuleT< CSmartCardRemovalModule, IDS_SERVICENAME >,
		public monitorObserver
{
	mutexObj lock;
	std::ofstream log;
public :
	CSmartCardRemovalModule() : lock("lock") {
		CHAR path[MAX_PATH];
		GetTempPathA(sizeof(path),path);
		strcat(path,"smartcardremoval.run.log");
		log.open(path);
		log << "module started" << std::endl;
		}

	bool ParseCommandLine(LPCTSTR lpCmdLine,HRESULT* pnRetCode ) throw( ) {
		if (!CAtlServiceModuleT<CSmartCardRemovalModule, IDS_SERVICENAME>::ParseCommandLine(lpCmdLine,pnRetCode))
			return false;

		TCHAR szTokens[] = _T("-/");

		LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
		while (lpszToken != NULL) {
			if (WordCmpI(lpszToken, _T("RemoveCerts"))==0) {
				log << "<<--launched with /RemoveCerts" << endl;
				removeCerts();
				return false;
				}
			lpszToken = FindOneOf(lpszToken, szTokens);
		}
		return true;
		}

	virtual void onEvent(monitorEvent eType,int param) {
		switch(eType) {
			case READERS_CHANGED:
				log << "readers changed" << std::endl;break;
			case NO_CARDS_LEFT:
				log << "no esteid cards" << std::endl;

				log << "launching cleanup" << std::endl;
				CHAR szFilePath[MAX_PATH];
				::GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
				ProcessStarter proc(szFilePath,"/RemoveCerts");
				proc.Run(true);
				break;
			}
		}

	DECLARE_LIBID(LIBID_SmartCardRemovalLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SMARTCARDREMOVAL, "{DEAE87CA-A84D-4F75-BC47-721D7F0F7848}")
	HRESULT InitializeSecurity() throw()
	{
		return S_OK;
	}
	void RunMessageLoop()
	{
		log << "starting monitor .." << std::endl;
		monitorThread monitor(*this,lock);
		monitor.start();
		MSG msg;
		log << "running .. " << std::endl;
		while (GetMessage(&msg, 0, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		log << "stopped." << std::endl;
	}

	struct certStore { //wrap store handle
		HCERTSTORE hStore;
	public:
		certStore(LPCTSTR storeName) {
			hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,0, (HCRYPTPROV)NULL,
					CERT_SYSTEM_STORE_CURRENT_USER /*| CERT_STORE_READONLY_FLAG  */
					| CERT_STORE_OPEN_EXISTING_FLAG,storeName);
			if (!hStore) throw std::runtime_error("cant open certstore");
		};
		~certStore() {CertCloseStore(hStore,0);};
		operator HCERTSTORE() const {return hStore;}
		};

	bool CertFromCSP(PCCERT_CONTEXT ctx) {
		std::vector<BYTE> buf;
		DWORD sz = -1;
		if (TRUE != CertGetCertificateContextProperty(ctx,
			CERT_KEY_PROV_INFO_PROP_ID,NULL,&sz)) return false;
		buf.resize(sz);
		if (TRUE != CertGetCertificateContextProperty(ctx,
			CERT_KEY_PROV_INFO_PROP_ID,&buf[0],&sz)) return false;
		CRYPT_KEY_PROV_INFO *info = (CRYPT_KEY_PROV_INFO *) &buf[0];
		if (info->pwszProvName == NULL) return false;
		if (std::wstring(info->pwszProvName) == std::wstring(L"EstEID NewCard CSP")) {
			log << "found EstEID NewCard CSP cert" << endl;
			return true;
			}
		return false;
		}

	void removeCerts() {
		log << "enuming certs" << endl;
		try {
			certStore my(_T("MY"));
			PCCERT_CONTEXT ctx = NULL;
			while (ctx = CertEnumCertificatesInStore(my,ctx) ) {
				CHAR nameBuf[MAX_PATH];
				CertGetNameStringA(ctx,CERT_NAME_FRIENDLY_DISPLAY_TYPE,
					0,NULL,nameBuf,sizeof(nameBuf));
				log << "checking if Esteid cert [" << nameBuf << "]" << endl;
				if (!CertFromCSP(ctx)) continue;

				PCCERT_CONTEXT dup = CertDuplicateCertificateContext(ctx);
				log << "erasing ctx .." << endl;
				BOOL suc = CertDeleteCertificateFromStore(dup);
				if (!suc) {
					DWORD gle = GetLastError();
					log << "we got an error on CertDeleteCertificateFromStore, " << gle << endl;
					}
				}
		} catch(...) {}
		log << "..done enum" << endl;
		}
};

CSmartCardRemovalModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

