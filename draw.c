#include "core.h"
#include "draw.h"
#include "gpu.h"

typedef struct Draw_Context Draw_Context;
struct Draw_Context
{
    Gpu_Handle rectTexture;
};

static Draw_Context s_DrawContext;

void Draw_Startup()
{
    u32 whitePixel = 0xFFFFFFFF;
    s_DrawContext.rectTexture = Gpu_CreateTexture(1, 1, &whitePixel);
}

void Draw_Shutdown()
{
    Gpu_FreeTexture(s_DrawContext.rectTexture);
}

void Draw_BeginFrame()
{
    Gpu_Clear(0, 0, 0);
}

void Draw_EndFrame()
{
    Gpu_Present();
}

void Draw_Text(int x, int y, const char *format, ...)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(format);

    /*
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
    */
}

void Draw_Rectangle(int x, int y, int w, int h, Draw_Color color)
{
    Gpu_Sprite sprite = 
    {
        .textureX = 0, .textureY = 0, .textureWidth = 1, .textureHeight = 1,
        .screenX = (float)x, .screenY = (float)y, .screenWidth = (float)w, .screenHeight = (float)h,
        .r = color.r, .g = color.g, .b = color.b,
    };

    Gpu_SpritePass pass;
    pass.spriteCount = 1;
    pass.sprites = &sprite;
    pass.texture = s_DrawContext.rectTexture;
    Gpu_SubmitSprites(pass);
}

void Draw_Line(int x1, int y1, int x2, int y2, Draw_Color color)
{
    UNUSED(x1);
    UNUSED(y1);
    UNUSED(x2);
    UNUSED(y2);
    UNUSED(color);
}

void Draw_Grid(int spacing)
{
    int thickness = 3;
    float tint = 0.125f;
    Draw_Color color = { .r = tint, .g = tint, .b = tint };

    int width, height; 
    Os_GetWindowSize(&width, &height);

    for (int x = 0; x < width; x += spacing)
        Draw_Rectangle(x, (int)(height * 0.5f), thickness, height, color);

    for (int y = 0; y < height; y += spacing)
        Draw_Rectangle((int)(width * 0.5f), y, width, thickness, color);
}
