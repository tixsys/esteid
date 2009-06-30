#pragma once

#ifdef WIN32
#define ESTEID_IMPORT_SPEC __declspec(dllimport)
#else
#define ESTEID_IMPORT_SPEC 
#endif

#ifdef WIN32
#define CK_CALL_SPEC __stdcall
#else
#define CK_CALL_SPEC
#endif

#ifdef ESTEIDREADER_EXPORTS
/* Specified that the function is an exported DLL entry point. */
#define ESTEID_EXPORT_SPEC extern "C" __declspec(dllexport)
#else
#define ESTEID_EXPORT_SPEC extern "C" ESTEID_IMPORT_SPEC
#endif

#define ESTEID_DEFINE_FUNCTION(returnType, name) \
  ESTEID_EXPORT_SPEC returnType CK_CALL_SPEC name

#define ESTEID_DECLARE_FUNCTION(returnType, name) \
  ESTEID_EXPORT_SPEC returnType CK_CALL_SPEC name

ESTEID_DEFINE_FUNCTION(HANDLE, OpenReader(char *module));
ESTEID_DEFINE_FUNCTION(void, CloseReader(HANDLE h));
ESTEID_DEFINE_FUNCTION(ulong, GetSlotCount(HANDLE h, int token_present));
ESTEID_DEFINE_FUNCTION(ulong, IsMechanismSupported(HANDLE h, ulong slot, ulong flag));
ESTEID_DEFINE_FUNCTION(ulong, GetTokenInfo(HANDLE h, ulong slot, CK_TOKEN_INFO_PTR info));
ESTEID_DEFINE_FUNCTION(ulong, Sign(HANDLE h, ulong slot, const char *pin, uchar *data, ulong dlength, 
                       void **hdata, ulong *hlength,
                       void **sdata, ulong *slength));
