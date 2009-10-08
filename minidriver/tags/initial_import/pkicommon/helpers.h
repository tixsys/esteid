#if !defined(HELPERS_H_INCLUDED)
#define HELPERS_H_INCLUDED

#include <string>
using namespace std;

#include <windows.h>

class hresultError: public runtime_error {
	HRESULT hr;
	const char *msg;
public:
	hresultError(HRESULT code,const char *p):hr(code),msg(p),runtime_error("hresultError") {}
	static void check(HRESULT err,const char*p);
	string getDesc() {
		char buf[1000];
		sprintf_s(buf,sizeof(buf)," exception:'%s' msg:'%s' code:'0x%08X'",
			runtime_error::what(),msg,hr);
		return buf;
		}

};

string WstrToStr(const wstring& iwstr);
wstring StrToWstr(const string& istr);

bool operator==(const wstring& iwstr, const string& istr);
bool operator==(const string& istr, const wstring& iwstr);
bool operator!=(const wstring& iwstr, const string& istr);
bool operator!=(const string& istr, const wstring& iwstr);

#endif