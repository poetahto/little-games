#include "os.c"

void Entrypoint()
{
    OS_InitWindow(800, 600);
    bool isRunning = true;

    while (isRunning)
    {
        OS_Event event;

        while (OS_PumpEvents(&event))
        {
            switch (event.type)
            {
                case OS_EVENT_QUIT: isRunning = false; break;
                case OS_EVENT_KEY_DOWN: 
                {
                    switch (event.key)
                    {
                        case OS_KEY_ESCAPE: isRunning = false; break;
                        case OS_KEY_UP: OS_Log("Pressed up"); break;
                        case OS_KEY_DOWN: OS_Log("Pressed down"); break;
                        case OS_KEY_LEFT: OS_Log("Pressed left"); break;
                        case OS_KEY_RIGHT: OS_Log("Pressed right"); break;
                        default: break;
                    }
                }
                default: break;
            }
        }

        OS_RenderClear();
        static int x = 10;
        const int SIZE = 25;
        OS_RenderRect(x += SIZE, SIZE, SIZE, SIZE);

        OS_Sleep(500);
    }

    OS_FreeWindow();
}
