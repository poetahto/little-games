#include <stdlib.h>

#ifdef OS_LINUX
    #include "os_linux.c"
    // NOTE(poe): X11 split into its own source file,
    // so it can be easily interchanged with wayland or
    // other window managers
    #include "os_linux_x11.c"
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

Arena Os_CreateArena(int capacity)
{
    return (Arena) { .buffer = Os_HeapAlloc(capacity), .capacity = capacity };
}

void Os_FreeArena(Arena arena)
{
    Os_HeapFree(arena.buffer);
}
