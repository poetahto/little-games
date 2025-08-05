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

void Draw_Rectangle(int x, int y, int w, int h)
{
    Os_RenderRect(x, y, w, h);
}
