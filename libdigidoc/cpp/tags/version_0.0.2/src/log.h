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
#define LOG_LEVEL_DEBUG 1
#define LOG_USE_COLORS  0



#undef ERROR
#undef WARN
#undef INFO
#undef DEBUG


#if defined (LOG_LEVEL_ERROR) && LOG_LEVEL_ERROR > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        #define ERROR(...) do {printf("\033[31mERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0);
    #else
        #define ERROR(...) do {printf("ERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
    #endif
#else
    #define ERROR(...)
#endif


#if defined (LOG_LEVEL_WARN) && LOG_LEVEL_WARN > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        #define WARN(...) do {printf("\033[33mWARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0);
    #else
        #define WARN(...) do {printf("WARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
    #endif
#else
    #define WARN(...)
#endif


#if defined (LOG_LEVEL_INFO) && LOG_LEVEL_INFO > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        #define INFO(...) do {printf("\033[36mINFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0);
    #else
        #define INFO(...) do {printf("INFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
    #endif
#else
    #define INFO(...)
#endif


#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
    #if defined (LOG_USE_COLORS) && LOG_USE_COLORS > 0
        //#define DEBUG(...) do {printf("\033[32mDEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\033[0m\n");} while(0);
        #define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
    #else
        #define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
    #endif
#else
    #define DEBUG(...)
#endif


#endif // !defined(__LOG_H_INCLUDED__)
