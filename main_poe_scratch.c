#include "os.c"

void Entrypoint()
{
    OS_Log("format test: %i", 10);

    OS_InitWindow(800, 600);
    OS_Log("Created window");

    bool isRunning = true;

    while (isRunning)
    {
        OS_Event event;

        while (OS_PumpEvents(&event))
        {
            switch (event.type)
            {
                case OS_EVENT_QUIT: isRunning = false; break;
            }
        }
    }

    OS_Sleep(500);

    OS_FreeWindow();
    OS_Log("Free window");
}
