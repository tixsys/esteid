// Contents of soapdefs.h 
//#include <ostream> 
#define SOAP BUFLEN 20480 // use large send/recv buffer
#define WITH_OPENSSL
#define HAVE_OPENSSL_SSL_H
//#define HAVE_CONFIG_H
#ifdef WIN32
#define SOAP_FMAC1 __declspec(dllexport)
#define SOAP_FMAC3 __declspec(dllexport)
#endif
