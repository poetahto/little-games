#include "os.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    Entrypoint();
    return 0;
}

int OS_Timestamp(void)
{
    return 0;
}

void OS_Log(const char *message)
{
    printf("%s\n", message);
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
    // TODO(poe): Impl
}
