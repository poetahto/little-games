#include "core.h"
#include "draw.h"
#include "os.h"

typedef struct Draw_Context Draw_Context;
struct Draw_Context
{
    Arena tempArena;
};

static Draw_Context sDrawContext;

void Draw_Startup()
{
    sDrawContext.tempArena = CreateArena(KB(16));
}

void Draw_Shutdown()
{
    FreeArena(sDrawContext.tempArena);
}

void Draw_BeginFrame()
{
    ArenaReset(&sDrawContext.tempArena);
    Os_RenderClear();
}

void Draw_EndFrame()
{
    // NOTE(poe): Nothing for now.
}

void Draw_Text(int x, int y, const char *format, ...)
{
    va_list args; 

    // Figure out how much space we need to store the format string
    va_start(args, format);
    int length = vsnprintf(NULL, 0, format, args);
    va_end(args);

    // Reserve space
    char *buffer = ArenaAlloc(&sDrawContext.tempArena, length + 1);
    buffer[length] = '\0';

    // Copy the finished string
    va_start(args, format);
    vsnprintf(buffer, length + 1, format, args);
    va_end(args);

    // Render
    Os_RenderText(x, y, buffer, length);
}

void Draw_Rectangle(int x, int y, int w, int h, Draw_Color color)
{
    Os_RenderSetColor(color.r, color.g, color.b);
    Os_RenderRect(x, y, w, h);
}

void Draw_Line(int x1, int y1, int x2, int y2)
{
    Os_RenderLine(x1, y1, x2, y2);
}

void Draw_Grid(int spacing)
{
    float tint = 0.1;
    Os_RenderSetColor(tint, tint, tint);
    Os_Size size = Os_GetWindowSize();

    for (int x = 0; x < size.width; x += spacing)
        Os_RenderLine(x, 0, x, size.height);

    for (int y = 0; y < size.height; y += spacing)
        Os_RenderLine(0, y, size.width, y);
}
