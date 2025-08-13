#include "core.h"
#include "draw.h"
#include "gpu.h"

typedef struct Draw_SpriteNode Draw_SpriteNode;
struct Draw_SpriteNode {
    Gpu_Sprite sprite;
    Draw_SpriteNode *next;
};

static Draw_SpriteNode *gDrawFirstSprite = NULL;
static Draw_SpriteNode *gDrawLastSprite = NULL;
static Gpu_Handle gDrawRectTexture;
static Arena gDrawArena;

void Draw_Startup()
{
    Gpu_Startup();
    gDrawArena = Os_CreateArena(MB(16));
    int whitePixel = 0xFFFFFFFF;
    Image rectImage = { .width = 1, .height = 1, .pixels = &whitePixel };
    gDrawRectTexture = Gpu_CreateTexture(rectImage);
}

void Draw_Shutdown()
{
    Os_FreeArena(gDrawArena);
    Gpu_FreeTexture(gDrawRectTexture);
    Gpu_Shutdown();
}

void Draw_BeginFrame()
{
    ArenaReset(&gDrawArena);
    Gpu_Clear(0, 0, 0);
}

void Draw_EndFrame()
{
    Gpu_SpritePass pass;
    pass.spriteCount = 0;
    pass.texture = gDrawRectTexture;
    Draw_SpriteNode *spriteIterator = gDrawFirstSprite;

    while (spriteIterator != NULL)
    {
        spriteIterator = spriteIterator->next;
        pass.spriteCount++;
    }

    Gpu_Sprite *sprites = ArenaAlloc(&gDrawArena, pass.spriteCount * sizeof(Gpu_Sprite));

    for (int i = 0; i < pass.spriteCount; i++)
        sprites[i] = (gDrawFirstSprite + i)->sprite;

    pass.sprites = sprites;
    Gpu_SubmitSprites(pass);
    Gpu_Present();
}

void Draw_Texture(Gpu_Handle texture, int x, int y, int width, int height, Draw_Color color)
{
    int textureWidth, textureHeight; 
    Gpu_GetTextureSize(texture, &textureWidth, &textureHeight);

    Gpu_Sprite sprite = {
        .textureX = 0, .textureY = 0, .textureWidth = textureWidth, .textureHeight = textureHeight,
        .screenX = (float)x, .screenY = (float)y, .screenWidth = (float)width, .screenHeight = (float)height,
        .r = color.r, .g = color.g, .b = color.b,
    };

    Gpu_SpritePass pass = {
        .spriteCount = 1,
        .sprites = &sprite,
        .texture = texture,
    };

    Gpu_SubmitSprites(pass);
}

void Draw_Rectangle(int x, int y, int width, int height, Draw_Color color)
{
    Gpu_Sprite sprite = {
        .textureX = 0, .textureY = 0, .textureWidth = 1, .textureHeight = 1,
        .screenX = (float)x, .screenY = (float)y, .screenWidth = (float)width, .screenHeight = (float)height,
        .r = color.r, .g = color.g, .b = color.b,
    };

    Draw_SpriteNode *node = ArenaAlloc(&gDrawArena, sizeof(Draw_SpriteNode));
    node->sprite = sprite;
    node->next = NULL;

    if (!gDrawFirstSprite || !gDrawLastSprite)
    {
        gDrawFirstSprite = gDrawLastSprite = node;
    }
    else
    {
        gDrawLastSprite->next = node;
        gDrawLastSprite = node;
    }
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
