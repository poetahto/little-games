#ifdef OS_LINUX
    #include "os_linux.c"
#endif

#ifdef OS_WIN32
    #error "No windows support"
#endif

#ifdef OS_APPLE
    #error "No apple support"
#endif
