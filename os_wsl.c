#include "os.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    Entrypoint();
    return 0;
}

void OS_Log(const char *message)
{
    printf("%s\n", message);
}
