#include "core.c"
#include "os.c"
#include "draw.c"
#include "gpu.c"
#include "profile.c"

void Entrypoint()
{
    Os_CreateWindow(800, 600);
    Gpu_Startup();
    Draw_Startup();

    int size = 50;
    bool isRunning = true;

    while (isRunning)
    {
        Os_WindowEvent event;

        while (Os_PumpWindowEvents(&event))
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
        Draw_Rectangle(x += size, size, size, size, DRAW_YELLOW);
        Draw_EndFrame();

        Os_Sleep(500);
    }

    Draw_Shutdown();
    Gpu_Shutdown();
    Os_FreeWindow();
}
