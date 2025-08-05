#include "core.c"
#include "os.c"
#include "draw.c"
#include "wm.c"

void Entrypoint()
{
    Wm_Startup(800, 600);
    Draw_Startup();

    int size = 50;
    bool isRunning = true;

    while (isRunning)
    {
        Wm_Event event;

        while (Wm_PumpEvents(&event))
        {
            switch (event.type)
            {
                case WM_EVENT_QUIT: isRunning = false; break;
                case WM_EVENT_KEY_DOWN: 
                {
                    switch (event.key)
                    {
                        case WM_KEY_ESCAPE: isRunning = false; break;
                        case WM_KEY_UP: Os_Log("Pressed up"); break;
                        case WM_KEY_DOWN: Os_Log("Pressed down"); break;
                        case WM_KEY_LEFT: Os_Log("Pressed left"); break;
                        case WM_KEY_RIGHT: Os_Log("Pressed right"); break;
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
    Wm_Shutdown();
}
