/*!
	\file		SCError.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author$
	\date		$Date$
*/
// Revision $Revision$
#pragma once

/// Exception class for smartcard subsystem errors
/** Smartcard subsystem errors, like reader busy etc. Currently these are only
 thrown for PCSCManager, but CTAPI should derive its own from here and throw them
 as well */
class SCError :
	public std::runtime_error
{
	std::string desc;
	const SCError operator=(const SCError &) {};
public:
	const long error; //SC Api returns longs
	SCError(long err);
	virtual ~SCError() throw() {}
	virtual const char * what() const throw() {	return desc.c_str();} 
	static void check(long err);
};
