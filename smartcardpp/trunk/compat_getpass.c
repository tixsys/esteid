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

#ifdef _WIN32

#include "compat_getpass.h"
#include <stdio.h>
#include <conio.h>

char *getpass(const char *prompt)
{
	static char buf[128];
	size_t i;

	fputs(prompt, stderr);
	fflush(stderr);
	for (i = 0; i < sizeof(buf) - 1; i++) {
		buf[i] = _getch();
		if (buf[i] == '\r')
			break;
	}
	buf[i] = 0;
	fputs("\n", stderr);
	return buf;
}

#endif
