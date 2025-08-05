#include "core.h"
#include "draw.h"
#include "wm.h"

typedef struct Draw_Context Draw_Context;
struct Draw_Context
{
    Arena tempArena;
};

static Draw_Context s_DrawContext;

void Draw_Startup()
{
    s_DrawContext.tempArena = CreateArena(KB(16));
}

void Draw_Shutdown()
{
    FreeArena(s_DrawContext.tempArena);
}

void Draw_BeginFrame()
{
    ArenaReset(&s_DrawContext.tempArena);
    Wm_RenderClear();
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
    char *buffer = ArenaAlloc(&s_DrawContext.tempArena, length + 1);
    buffer[length] = '\0';

    // Copy the finished string
    va_start(args, format);
    vsnprintf(buffer, length + 1, format, args);
    va_end(args);

    // Render
    Wm_RenderText(x, y, buffer, length);
}

void Draw_Rectangle(int x, int y, int w, int h, Draw_Color color)
{
    Wm_RenderSetColor(color.r, color.g, color.b);
    Wm_RenderRect(x, y, w, h);
}

void Draw_Line(int x1, int y1, int x2, int y2)
{
    Wm_RenderLine(x1, y1, x2, y2);
}

void Draw_Grid(int spacing)
{
    float tint = 0.1;
    Wm_RenderSetColor(tint, tint, tint);

    int width, height; 
    Wm_GetWindowSize(&width, &height);

    for (int x = 0; x < width; x += spacing)
        Wm_RenderLine(x, 0, x, height);

    for (int y = 0; y < height; y += spacing)
        Wm_RenderLine(0, y, width, y);
}
