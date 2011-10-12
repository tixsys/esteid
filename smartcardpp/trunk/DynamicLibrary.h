/*
* SMARTCARDPP
* 
* This software is released under either the GNU Library General Public
* License (see LICENSE.LGPL) or the BSD License (see LICENSE.BSD).
* 
* Note that the only valid version of the LGPL license as far as this
* project is concerned is the original GNU Library General Public License
* Version 2.1, February 1999
*
*/


#pragma once

#include <string>

/// Wraps a dynamically loaded system library
/** Dynamiclibrary loads a DLL or windows or .so on linux/others, specified by its
 basename, like "winscard" or "pcsclite". Somewhat hackish and does not really
 belong here */
struct DynamicLibrary {
	typedef void (*fProc)();
	void *mLibhandle;
	std::string name;
	const char *m_pathHint;
	bool construct(int version = 1,bool do_throw=true);
	static std::string arrPaths[];
	bool m_construct;
public:
	/// Constructor, loads the library specified by dllName
	DynamicLibrary(const char *dllName);
	/// Constructor, loads the library specified by dllName and version, version is only used on linux
	DynamicLibrary(const char *dllName,int version);
	/// Constructor, loads the library specified by dllName, using subdirectory hint
	DynamicLibrary(const char *dllName,const char *pathHint,int version=1,bool do_throw = true);
	~DynamicLibrary();
	/// get a procedure address from the library
	fProc getProc(const char *procName);
	/// get version string, returns resource version string of DLL on win32, and full path of library on linux
	std::string getVersionStr();
	/// return loaded library name
	std::string getName() { return name;}
	/// exists, returns false when constructed with do_throw=false, and there was an error
	bool exists();
	};
