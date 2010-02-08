#ifdef _WIN64
#define PKG_BIT " ( 64-bit )"
#else
#define PKG_BIT ""
#endif
#ifdef _DEBUG
#ifdef AVOID_SERVICE_LOAD
#define PKG_BUILD " ( debug - doesnt load in services)"
#else
#define PKG_BUILD " ( debug )"
#endif
#else
#define PKG_BUILD ""
#endif

/* Define to the full name of this package. */
#define PACKAGE_NAME "EstEID CSP" PKG_BIT PKG_BUILD

/* rc version */
#define RC_VERSION 2,2,19

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.2.19"

/* Name of package */
#define PACKAGE "esteidcsp.dll"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION

#define CRYPT_SIG_RESOURCE_NUMBER        0x29A
#define CRYPT_SIG_RESOURCE        TEXT("#666")
