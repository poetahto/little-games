#include <stdlib.h>

#ifdef OS_LINUX
    #include "os_linux.c"
#endif

#ifdef OS_WIN32
    #include "os_win32.c"
#endif

#ifdef OS_APPLE
    #error "No apple support"
#endif

void * Os_HeapAlloc(int sizeBytes)
{
    return malloc(sizeBytes);
}

void Os_HeapFree(void *buffer)
{
    free(buffer);
}
