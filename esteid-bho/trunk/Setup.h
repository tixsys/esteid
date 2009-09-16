
#ifdef _WIN64
#define PKG_BIT " ( 64-bit )"
#else
#define PKG_BIT ""
#endif
#ifdef _DEBUG
#define PKG_BUILD " ( debug )"
#else
#define PKG_BUILD ""
#endif

/* Define to the full name of this package. */
#define PACKAGE_NAME "EstEID Browser Signing plugin" PKG_BIT PKG_BUILD

/* rc version */
#define RC_VERSION 0,9,0

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.9.0"

/* Name of package */
#define PACKAGE "EstEIDSigningPluginBHO.dll"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION
