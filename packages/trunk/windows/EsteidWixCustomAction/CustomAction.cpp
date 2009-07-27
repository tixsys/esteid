
#include "stdafx.h"
#include "Certificates.h"


UINT __stdcall DetectJuurSK(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;

	hr = WcaInitialize(hInstall, "DetectJuurSK");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	if (findJuurSK()) {
		WcaLog(LOGMSG_STANDARD, "Juur-SK certificate already installed.");

		hr = WcaSetIntProperty(L"JUURCERTIFICATEINSTALLED", 1);
		ExitOnFailure(hr, "Failed to set JUURCERTIFICATEINSTALLED property value");
	} else {
		WcaLog(LOGMSG_STANDARD, "Juur-SK certificate is not installed.");
	}

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall RemoveCertificates(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;

	hr = WcaInitialize(hInstall, "RemoveCertificates");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	traverseStore(L"CA");
	WcaLog(LOGMSG_STANDARD, "Finished cleaning up certificate store.");

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}


// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
	__in HINSTANCE hInst,
	__in ULONG ulReason,
	__in LPVOID
	)
{
	switch(ulReason)
	{
	case DLL_PROCESS_ATTACH:
		WcaGlobalInitialize(hInst);
		break;

	case DLL_PROCESS_DETACH:
		WcaGlobalFinalize();
		break;
	}

	return TRUE;
}
