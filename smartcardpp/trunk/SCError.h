/*!
	\file		SCError.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-03-29 23:12:12 +0300 (Sun, 29 Mar 2009) $
*/
// Revision $Revision: 207 $
#pragma once

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
