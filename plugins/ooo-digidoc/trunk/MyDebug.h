#include <stdio.h>
#include <stdarg.h>

#define DEBUG 0

#ifdef _WIN32
#define DEBUG_FILE "c:\\OOoDebug.txt"
#define UNO_URL_HEAD "file:///"
#define SLASH "/"

#else
#define UNO_URL_HEAD "file://"
#define SLASH ""
#define DEBUG_FILE "/tmp/OOoDebug.txt"
#endif

#if DEBUG
#define PRINT_DEBUG(...)									\
	do {													\
			FILE *fp;										\
			fp = fopen(DEBUG_FILE, "a");					\
			fprintf(fp, "[%s:%d] in Function: %s\n",__FILE__, __LINE__, __FUNCTION__);	\
			fprintf(fp, __VA_ARGS__);						\
			fprintf(fp, "\n\n");							\
			fclose(fp);										\
        } while (0)											
#else
#define PRINT_DEBUG(...)           do { } while (0)
#endif

