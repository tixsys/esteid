#ifndef __COMPAT_GETPASS_H
#define __COMPAT_GETPASS_H

#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif
char *getpass (const char *prompt);
#ifdef __cplusplus
}
#endif
#else
#include <unistd.h>
#endif

#endif
