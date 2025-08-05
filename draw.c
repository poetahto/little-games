#include "draw.h"
#include "os.h"

void Draw_BeginFrame()
{
    Os_RenderClear();
}

void Draw_EndFrame()
{
    // NOTE(poe): Nothing for now.
}

void Draw_Rectangle(int x, int y, int w, int h)
{
    Os_RenderRect(x, y, w, h);
}
