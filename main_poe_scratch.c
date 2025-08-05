#include "core.c"
#include "os.c"
#include "draw.c"

void Entrypoint()
{
    // Subsystem startup
    Draw_Startup();

    Os_InitWindow(800, 600);

    int size = 50;

    bool isRunning = true;
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
        Draw_Grid(size);
        static int x = 0;
        Draw_Rectangle(x += size, size, size, size);
        Draw_EndFrame();

        Os_Sleep(500);
    }

    Os_FreeWindow();

    // Subsystem shutdown
    Draw_Shutdown();
}
