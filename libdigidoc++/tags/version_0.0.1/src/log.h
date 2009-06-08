#if !defined(__LOG_H_INCLUDED__)
#define __LOG_H_INCLUDED__



#undef LOG_LEVEL_ERROR
#undef LOG_LEVEL_WARN
#undef LOG_LEVEL_INFO
#undef LOG_LEVEL_DEBUG

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 1



#undef ERROR
#undef WARN
#undef INFO
#undef DEBUG


#if defined (LOG_LEVEL_ERROR) && LOG_LEVEL_ERROR > 0
    #define ERROR(...) do {printf("ERROR [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
#else
    #define ERROR(...)
#endif


#if defined (LOG_LEVEL_WARN) && LOG_LEVEL_WARN > 0
    #define WARN(...) do {printf("WARN  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
#else
    #define WARN(...)
#endif


#if defined (LOG_LEVEL_INFO) && LOG_LEVEL_INFO > 0
    #define INFO(...) do {printf("INFO  [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
#else
    #define INFO(...)
#endif


#if defined (LOG_LEVEL_DEBUG) && LOG_LEVEL_DEBUG > 0
    #define DEBUG(...) do {printf("DEBUG [%s:%d] - ",__FILE__, __LINE__);printf(__VA_ARGS__);printf("\n");} while(0);
#else
    #define DEBUG(...)
#endif


#endif // !defined(__LOG_H_INCLUDED__)
