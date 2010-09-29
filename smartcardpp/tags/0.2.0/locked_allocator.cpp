#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <string.h>
#endif
#include "common.h"

void * doAlloc(size_t n, void* hint) {
	void * ret = new unsigned char[n];
    if (!ret)
            throw std::bad_alloc();
#ifdef _WIN32
	VirtualLock(ret,n);
#else
	mlock(ret,n);
#endif
	return ret;
	}

void doFree(void *pmem,size_t n) {
#ifdef _WIN32
	VirtualUnlock(pmem,n);
#else
	munlock(pmem,n);
#endif
	memset(pmem,0,n);
	unsigned char * cp = (unsigned char *)pmem;
	delete [] cp;
	}
