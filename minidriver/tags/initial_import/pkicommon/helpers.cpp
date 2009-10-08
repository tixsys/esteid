#include "StdAfx.h"

#include "helpers.h"

void hresultError::check(HRESULT err,const char*p) {
		if (FAILED(err)) throw hresultError(err,p); 
		}

string WstrToStr(const wstring& iwstr)
{
 string str;
 for(wstring::const_iterator it = iwstr.begin(); it != iwstr.end(); ++it)
 {
  char ch = static_cast<char>(*it);
  str += ch;
 } return str;
}

wstring StrToWstr(const string& istr)
{
 wstring wstr;
 for(string::const_iterator it = istr.begin(); it != istr.end(); ++it)
 {
  wstr += *it;
 } 
 return wstr;
}

bool operator==(const wstring& iwstr, const string& istr)
{
 return WstrToStr(iwstr) == istr;
}

bool operator==(const string& istr, const wstring& iwstr)
{
 return iwstr == istr;
}

bool operator!=(const wstring& iwstr, const string& istr)
{
 return !(iwstr == istr);
}

bool operator!=(const string& istr, const wstring& iwstr)
{
 return !(istr == iwstr);
}
