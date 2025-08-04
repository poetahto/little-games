#include "os.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    Entrypoint();
    return 0;
}

int OS_Timestamp(void)
{
    return 0;
}

void OS_Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    putchar('\n');
    va_end(args);
}

void OS_InitWindow(int width, int height)
{
    // TODO(poe): Impl
}

void OS_FreeWindow()
{
    // TODO(poe): Impl
}

void OS_Sleep(int milliseconds)
{
    int microseconds = milliseconds * 1000;
    usleep(microseconds);
}
