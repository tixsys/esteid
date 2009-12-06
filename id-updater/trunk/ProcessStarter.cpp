// http://www.codeproject.com/KB/vista-security/interaction-in-vista.aspx
//#define NO_PROCESS
#include "ProcessStarter.h"

#include <windows.h>
#include <winbase.h>
#include "userenv.h"
#include "wtsapi32.h"
#include "winnt.h"
#include <Tlhelp32.h>
#include <smartcardpp/DynamicLibrary.h>

#include <iomanip>

ProcessStarter::ProcessStarter(const std::string& processPath, const std::string& arguments)
: processPath_(processPath), arguments_(arguments) 
{
#ifndef NOLOG
	char tpath[MAX_PATH];
	GetTempPathA(sizeof(tpath),tpath);
	strcat(tpath,"process-starter.log");
	log.open(tpath);
	log << "began log .. " << std::endl;
#endif
}

ProcessStarter::~ProcessStarter() {
	log << "end log" << std::endl;
}

#if !defined(WIN32) || defined(NO_PROCESS)
bool ProcessStarter::Run() {return false;}
#else

//#pragma comment(lib,"WtsApi32")
#pragma comment(lib,"Userenv")

HANDLE curTok = INVALID_HANDLE_VALUE;
HANDLE primTok = INVALID_HANDLE_VALUE;

ProcessStarter::_phandle ProcessStarter::GetCurrentUserToken()
{
	DynamicLibrary wts("Wtsapi32");
	BOOL (__stdcall * pWTSQueryUserToken)(ULONG SessionId,PHANDLE phToken) =
		(BOOL(__stdcall *)(ULONG,PHANDLE)) wts.getProc("WTSQueryUserToken");
	BOOL (__stdcall * pWTSEnumerateSessionsW)(HANDLE hServer,DWORD Reserved,DWORD Version,
			PWTS_SESSION_INFOW * ppSessionInfo,DWORD * pCount) = 
		(BOOL(__stdcall *)(HANDLE, DWORD,DWORD,PWTS_SESSION_INFOW *, DWORD *))
			wts.getProc("WTSEnumerateSessionsW");

    PHANDLE currentToken = &curTok;
    PHANDLE primaryToken = &primTok;

    int dwSessionId = 0;

    PWTS_SESSION_INFOW pSessionInfo = 0;
    DWORD dwCount = 0;

    pWTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);

    for (DWORD i = 0; i < dwCount; ++i)
    {
        WTS_SESSION_INFO si = pSessionInfo[i];
        if (WTSActive == si.State)
        {
            dwSessionId = si.SessionId;
            break;
        }
    }

    BOOL bRet = pWTSQueryUserToken(dwSessionId, currentToken);
    int errorcode = GetLastError();
    if (bRet == false)
    {
        return 0;
    }

    bRet = DuplicateTokenEx(*currentToken, TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, primaryToken);
    errorcode = GetLastError();
    if (bRet == false)
    {
        return 0;
    }

    return primaryToken;
}

unsigned long ProcessStarter::getShellProcessPID() {
	HANDLE hProcessSnap;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE ) 
		throw std::runtime_error("CreateToolhelp32Snapshot");
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	if( !Process32First( hProcessSnap, &pe32 ) )
		throw std::runtime_error( "Process32First" );  // Show cause of failure
	do  {
		if (std::wstring(pe32.szExeFile) == L"explorer.exe") {
			CloseHandle( hProcessSnap );
			return pe32.th32ProcessID;
			}
	  } while( Process32Next( hProcessSnap, &pe32 ) );
	CloseHandle( hProcessSnap );
	throw std::runtime_error("explorer process not found");
	}

HRESULT GetProcessToken(DWORD dwProcessID, LPHANDLE token, DWORD nUserNameMax, LPWSTR szwUserName, DWORD nUserDomainMax, LPWSTR szwUserDomain) {
	HANDLE hProcess=OpenProcess(PROCESS_DUP_HANDLE|PROCESS_QUERY_INFORMATION,TRUE,dwProcessID); 
	if (!hProcess) throw std::runtime_error("OpenProcess failed");
	HRESULT retval = S_OK;
	HANDLE hToken = INVALID_HANDLE_VALUE;
	if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken)) retval = HRESULT_FROM_WIN32(GetLastError());
	else {
		BYTE buf[MAX_PATH]; DWORD dwRead = 0;
		if (!GetTokenInformation(hToken, TokenUser, buf, MAX_PATH, &dwRead)) retval = HRESULT_FROM_WIN32(GetLastError());
		else {
			TOKEN_USER *puser = reinterpret_cast<TOKEN_USER*>(buf);
			SID_NAME_USE eUse;
			if (!LookupAccountSid(NULL, puser->User.Sid, szwUserName, &nUserNameMax, szwUserDomain, &nUserDomainMax, &eUse))
				retval = HRESULT_FROM_WIN32(GetLastError());
			}
		if (FAILED(retval)) return retval;
		if (!DuplicateTokenEx(hToken, 
			TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE, 
			NULL, SecurityImpersonation, TokenPrimary,token))
			retval = HRESULT_FROM_WIN32(GetLastError());
		else  retval = S_OK;
		CloseHandle(hToken);
		}
	return retval;
}

ProcessStarter::_phandle ProcessStarter::GetCurrentUserTokenOld()
{
	DWORD pid = getShellProcessPID();
	log << "ProcessStarter::shellprocessPID is " << pid << std::endl;
	TCHAR username[MAX_PATH] = { L'\0' };
	TCHAR domain[MAX_PATH]= { L'\0' }; 
	DWORD szUsername = MAX_PATH, szDomain = MAX_PATH;
	GetProcessToken(pid,&primTok,szUsername,username,szDomain,domain);
	log << "ProcessStarter::szUserName is ";
	char * pn = (char*)&username[0];
	while(*pn) {log << *pn; pn++; pn++;}
	log << std::endl;
    return &primTok;
}

bool ProcessStarter::Run(bool forceRun)
{
	OSVERSIONINFO version = {sizeof(OSVERSIONINFO)};

	GetVersionEx(&version);
	if ((version.dwMajorVersion == 5 && version.dwMinorVersion == 0 ) && !forceRun) { //dont need this on XP/Win2K 
		log << "not being forced to run .." << std::endl;
		return false;
		}

	char winDir[260];
	GetSystemDirectoryA(winDir,sizeof(winDir));

	PHANDLE primaryToken = 0;
	try {
		if ((version.dwMajorVersion == 5) && (version.dwMinorVersion == 0) ) //win2K
			primaryToken = GetCurrentUserTokenOld();
		else
			primaryToken = GetCurrentUserToken();
	} catch(std::exception &ex) {
		log << "exception :" << ex.what() << std::endl;
		}
    if (primaryToken == 0)
    {
		log << "primtok = 0" << std::endl;
		if (!forceRun) 
	        return false;

    }

    STARTUPINFOA StartupInfo;
    PROCESS_INFORMATION processInfo;
	memset(&StartupInfo,0,sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
	if (version.dwMajorVersion <= 5)
		StartupInfo.lpDesktop = "winsta0\\default";
#if 0
	std::string command = std::string("\"") + winDir + 
			"\\cmd.exe\" /C \""+ processPath_ + " " + arguments_ + "\"";
#else
    std::string command = "\"" + processPath_ + "\"";
    if (arguments_.length() != 0)
    {
        command += " " + arguments_;
    }
#endif
	log << "command:'" << command << "'" << std::endl;

    void* lpEnvironment = NULL;
	if (!forceRun) {
		BOOL resultEnv = CreateEnvironmentBlock(&lpEnvironment, *primaryToken, TRUE);
		if (resultEnv == 0)
		{                                
			long nError = GetLastError();                                
		}
		log << "CreateEnvironmentBlock ok" << std::endl;
		}

	BOOL result;
	if (!primaryToken) {
		result = CreateProcessA(0,(LPSTR)(command.c_str()), 
			NULL,NULL,
			FALSE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, 
			lpEnvironment, 0, &StartupInfo, &processInfo);
		}
	else {
		log << "creating as user " << std::endl;
		result = CreateProcessAsUserA(*primaryToken, 0, (LPSTR)(command.c_str()), 
			NULL,NULL,
			FALSE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, 
			lpEnvironment, 0, &StartupInfo, &processInfo);
		}

	if (result != FALSE) {
		log << "launched PID=" << processInfo.dwProcessId << std::endl;
		}
	else {
		log << "didnt launch" << std::endl;
		log << "CreateProcessAsUserA " << result << " err 0x" 
			<< std::hex << std::setfill('0') << std::setw(8) << GetLastError() << std::endl;
		}
    if (!forceRun) 
		CloseHandle(primaryToken);
    return (result != FALSE);
}
#endif
