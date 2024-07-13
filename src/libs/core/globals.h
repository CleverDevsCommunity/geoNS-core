#ifndef GEONS_GLOBAL_H
#define GEONS_GLOBAL_H 1
    #ifdef __unix__
        #define OS_WINDOWS 0
        #define OS_LINUX 0
        #define OS_FREE_BSD 0
        #define OS_SOLARIS 0
        
        #ifdef __linux__
            #undef OS_LINUX
            #define OS_LINUX 1
        #elif defined(__FreeBSD__)
            #undef OS_FREE_BSD
            #define OS_FREE_BSD 1
        #elif defined(sun) || defined(__sun)
            #undef OS_SOLARIS
            #define OS_SOLARIS 1
        #endif
        
    #elif defined(_WIN32) || defined(WIN32)
        #define OS_WINDOWS 1
    #endif
#endif // !GEONS_GLOBAL_H