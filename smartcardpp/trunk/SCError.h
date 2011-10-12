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

#include <stdexcept>

/// Exception class for smartcard subsystem errors
/** Smartcard subsystem errors, like reader busy etc. Currently these are only
 thrown for PCSCManager, but CTAPI should derive its own from here and throw them
 as well */
class SCError :
	public std::runtime_error
{
	const SCError operator=(const SCError &);
protected:
	std::string desc;
public:
	const long error; //SC Api returns longs
	SCError(long err);
	virtual ~SCError() throw() {}
	virtual const char * what() const throw() {	return desc.c_str();} 
	static void check(long err);
};
