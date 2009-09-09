/*!
	\file		netObj.h
	\copyright	(c) Kaido Kert ( kaidokert@gmail.com )    
	\licence	BSD
	\author		$Author: kaidokert $
	\date		$Date: 2009-07-07 08:35:50 +0300 (Tue, 07 Jul 2009) $
*/
// Revision $Revision: 346 $

enum connType {
	HTTP,
	HTTPS,
	FTP
};

#ifdef _WIN32 
#include "winInetObj.h"
typedef winInetObj netObj;
typedef inetConnect netConnect;
#else
#include "opensslObj.h"
typedef opensslObj netObj;
typedef opensslConnect netConnect;
#endif
