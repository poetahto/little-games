#include "core.c"
#include "os.c"
#include "draw.c"

void Entrypoint()
{
    Os_InitWindow(800, 600);
    bool isRunning = true;

    u8 size;
    Os_Random(&size, sizeof(size));
    size %= 50;
    Os_Log("size=%i", size);

    while (isRunning)
    {
        Os_Event event;

        while (Os_PumpEvents(&event))
        {
            switch (event.type)
            {
                case OS_EVENT_QUIT: isRunning = false; break;
                case OS_EVENT_KEY_DOWN: 
                {
                    switch (event.key)
                    {
                        case OS_KEY_ESCAPE: isRunning = false; break;
                        case OS_KEY_UP: Os_Log("Pressed up"); break;
                        case OS_KEY_DOWN: Os_Log("Pressed down"); break;
                        case OS_KEY_LEFT: Os_Log("Pressed left"); break;
                        case OS_KEY_RIGHT: Os_Log("Pressed right"); break;
                        default: break;
                    }
                }
                default: break;
            }
        }

        Draw_BeginFrame();
        static int x = 10;
        Draw_Rectangle(x += size, size, size, size);
        Draw_EndFrame();

        Os_Sleep(500);
    }

    Os_FreeWindow();
}
