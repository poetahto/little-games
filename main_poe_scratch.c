#include "os.c"

void Entrypoint()
{
    OS_Log("format test: %i", 10);

    OS_InitWindow(800, 600);
    OS_Log("Created window");

    OS_Sleep(500);

    OS_FreeWindow();
    OS_Log("Free window");
}
