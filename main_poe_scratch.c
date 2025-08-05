#include "os.c"

void Entrypoint()
{
    OS_InitWindow(800, 600);
    bool isRunning = true;

    u8 size;
    OS_Random(&size, sizeof(size));
    size %= 50;
    OS_Log("size=%i", size);

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
        OS_RenderRect(x += size, size, size, size);

        OS_Sleep(500);
    }

    OS_FreeWindow();
}
