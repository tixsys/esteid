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
