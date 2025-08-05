#include "os.h"
#include "core.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/random.h>

int main()
{
    Entrypoint();
    return 0;
}

void Os_Sleep(int milliseconds)
{
    int microseconds = milliseconds * 1000;
    usleep(microseconds);
}

void Os_Random(void *buffer, int bufferLength)
{
   ssize_t count = getrandom(buffer, bufferLength, 0);

   // NOTE(poe): I don't entirely understand when this could fail, hence 
   // this curiosity-driven assert.
   assert(count == bufferLength);
}

void Os_Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    putchar('\n');
    va_end(args);
}
