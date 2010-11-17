/*!
	\file		DynamicLibrary.cpp
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-10-05 22:16:29 +0300 (E, 05 okt 2009) $
*/
// Revision $Revision: 473 $
#include "DynamicLibrary.h"
#include <string.h>
#include "common.h"

DynamicLibrary::DynamicLibrary(const char *dllName) :
	mLibhandle(NULL),name(dllName),m_pathHint("") {
	m_construct = construct();
	}

DynamicLibrary::DynamicLibrary(const char *dllName,int version) :
	mLibhandle(NULL),name(dllName),m_pathHint("") {
	m_construct = construct(version);
	}

DynamicLibrary::DynamicLibrary(const char *dllName,const char *pathHint,
	int version,bool do_throw) : mLibhandle(NULL),name(dllName) {
	m_pathHint = pathHint;
	m_construct = construct(version , do_throw);
	}

bool DynamicLibrary::exists() {
	return mLibhandle != NULL;
	}

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib,"version")

bool DynamicLibrary::construct(int , bool do_throw) {
	mLibhandle = LoadLibraryA(name.c_str());
	if (!mLibhandle)
		mLibhandle = LoadLibraryA( std::string(std::string(m_pathHint) + "\\" + name).c_str());
	if (!mLibhandle) {
		std::ostringstream buf;
		buf << "Dynamic library '" << name << "' not found in system";
		if (do_throw)
		  throw std::runtime_error(buf.str());
		else 
		  return false;
		}
	return true;
	}

DynamicLibrary::~DynamicLibrary() {
	FreeLibrary((HMODULE)mLibhandle);
	}

DynamicLibrary::fProc DynamicLibrary::getProc(const char *procName) {
	fProc proc =(fProc) GetProcAddress((HMODULE)mLibhandle,procName);
	if (!proc) throw std::runtime_error("proc not found");
	return proc;
	}

std::string DynamicLibrary::getVersionStr() {
	std::string ret = "missing";
	DWORD infoHandle;
	LONG sz = GetFileVersionInfoSizeA(name.c_str(),&infoHandle);
	if (!sz) return ret;

	VS_FIXEDFILEINFO * fileInf;
	std::vector<BYTE> buf(sz*2);
	if (!GetFileVersionInfoA(name.c_str(),0,sz,&buf[0])) return ret;
	UINT len;
	if (!VerQueryValueA(&buf[0],"\\",(LPVOID *) &fileInf,&len)) return ret;

	std::ostringstream strb;
	strb << HIWORD(fileInf->dwFileVersionMS) << "."
		 << LOWORD(fileInf->dwFileVersionMS) << "."
		 << HIWORD(fileInf->dwFileVersionLS) << "."
		 << LOWORD(fileInf->dwFileVersionLS);
	return strb.str();
	}

#endif //_WIN32

#if !defined(_WIN32)
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

std::string DynamicLibrary::arrPaths[] = { "","/lib/","/usr/local/lib/","/usr/pkg/lib","/usr/lib/"
	,"/lib64/","/usr/lib64/","/usr/lib/engines/"
#if defined(__APPLE__) 
	,"/Library/OpenSC/lib/engines/", "/Library/Frameworks/", "/System/Library/Frameworks/"
#endif
	};

#include <iostream>

bool DynamicLibrary::construct(int version,bool do_throw) {
	size_t i,j;
	std::ostringstream buf;
	buf << version;
	std::string arrStr[] = {
			name,
			name + ".so",
		"lib" + name + ".so",
			name + ".so." + buf.str(),
		"lib" + name + ".so." + buf.str(),
#if defined(__APPLE__)
		name + ".dylib",
		"lib" + name + ".dylib",
			name + "." + buf.str() + ".dylib",
		"lib" + name + "." + buf.str() + ".dylib",
		name + ".framework/" + name,
#endif
		},search,qname;
	mLibhandle = NULL;
	for(j = 0;j < sizeof(arrPaths) / sizeof(*arrPaths);j++) {
		for(i = 0;i < sizeof(arrStr) / sizeof(*arrStr);i++) {
			qname = arrPaths[j] + arrStr[i];

			search+= qname + ",";
			mLibhandle=dlopen(qname.c_str(),RTLD_LAZY);
			if (mLibhandle) break;

			qname = arrPaths[j] + m_pathHint + "/" + arrStr[i];
			search+= qname + ",";
			mLibhandle=dlopen(qname.c_str(),RTLD_LAZY);
			if (mLibhandle) break;
			}
		if (mLibhandle) break;
		}
	if (!mLibhandle) {
		buf.str("");
		buf << "Dynamic library '" << name << "' not found in system";
		if (do_throw) 
		  throw std::runtime_error(buf.str());
		else
		  return false;
		}
	name = arrStr[i];
	return true;
	}

DynamicLibrary::~DynamicLibrary() {
	}

DynamicLibrary::fProc DynamicLibrary::getProc(const char *procName) {
	std::ostringstream buf;
	dlerror();
	void * ptr = dlsym(mLibhandle,procName);
	fProc proc = NULL;
	memcpy(&proc,&ptr,sizeof(ptr)); //hack around not being able to copy void to fn ptr
	if (dlerror() == NULL)
		return proc;
	buf << "proc not found:" << procName;
	throw std::runtime_error(buf.str().c_str());
	}

void tryReadLink(std::string name,std::string path,std::string &result) {
	if (!result.empty()) return;
	char buffer[1024];
	memset(buffer,0,sizeof(buffer));
	int link = readlink(std::string(path+name).c_str(),buffer,sizeof(buffer));
	if (-1!= link) {
		if(std::string(buffer).rfind("/") != std::string::npos)
			result = buffer;
		else
			result = path + buffer;
		return;
		}
	struct stat buff;
	int file = stat(std::string(path+name).c_str(),&buff);
	if (-1!=file)
		result = path + name;
	}

//this is a hack, but should work most of the time. any way to ask dlopen for the actual file used ?
std::string DynamicLibrary::getVersionStr() {
	std::string result;
	for(size_t i = 0;i < sizeof(arrPaths) / sizeof(*arrPaths);i++) {
		tryReadLink(name,arrPaths[i],result);
		tryReadLink(name,arrPaths[i] + m_pathHint + "/",result);
		if(!result.empty()) return result;
		}
	if (result.empty()) result = "unknown";
	return result;
	}
#endif
