#ifdef WIN32
#define EXP_DIGIDOC __declspec(dllexport)
#pragma warning( disable: 4251 ) // shut up std::vector warnings
#else
#define EXP_DIGIDOC
#endif
