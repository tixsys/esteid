#ifdef WIN32
  #ifdef digidocpp_EXPORTS
    #define EXP_DIGIDOC __declspec(dllexport)
  #else
    #define EXP_DIGIDOC __declspec(dllimport)
  #endif
  #pragma warning( disable: 4251 ) // shut up std::vector warnings
#else
  #if __GNUC__ >= 4
    #define EXP_DIGIDOC __attribute__ ((visibility("default")))
  #else
    #define EXP_DIGIDOC
  #endif
#endif
