#if !defined(__LOG_H_INCLUDED__)
#define __LOG_H_INCLUDED__



#undef LOG_LEVEL_ERROR
#undef LOG_LEVEL_WARN
#undef LOG_LEVEL_INFO
#undef LOG_LEVEL_DEBUG
#undef LOG_USE_COLORS

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 0
#define LOG_USE_COLORS  0



#undef ERR
#undef WARN
#undef INFO
#undef DEBUG


#if defined (LOG_LEVEL_ERROR) && LOG_LEVEL_ERROR > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        #define ERR(...) do {printf("\033[31mERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
    #else
        #define ERR(...) do {printf("ERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
    #endif
#else
    #define ERR(...)
#endif


#if defined (LOG_LEVEL_WARN) && LOG_LEVEL_WARN > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        #define WARN(...) do {printf("\033[33mWARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
    #else
        #define WARN(...) do {printf("WARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
    #endif
#else
    #define WARN(...)
#endif


#if defined (LOG_LEVEL_INFO) && LOG_LEVEL_INFO > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        #define INFO(...) do {printf("\033[36mINFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
    #else
        #define INFO(...) do {printf("INFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
    #endif
#else
    #define INFO(...)
#endif


#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        //#define DEBUG(...) do {printf("\033[32mDEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0)
        #define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
    #else
        #define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0)
    #endif
#else
    #define DEBUG(...)
#endif

#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
    #define DEBUGMEM(msg, ptr, size) dbgPrintfMemImpl(msg, ptr, size, __FILE__, __LINE__)
    void dbgPrintfMemImpl(char const * msg, void const * ptr, int size, char const* file, int line);
    void HD(unsigned char *p,int n);
#else
    #define DEBUGMEM(msg, ptr, size)
#endif

#endif // !defined(__LOG_H_INCLUDED__)
