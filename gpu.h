#ifndef GAMES_GPU_H
#define GAMES_GPU_H

typedef union Gpu_Handle Gpu_Handle;
union Gpu_Handle
{
    void *dataPtr;
    unsigned int dataUint;
};

typedef struct Gpu_Sprite Gpu_Sprite;
struct Gpu_Sprite
{
    float textureX, textureY, textureWidth, textureHeight;
    float screenX, screenY, screenWidth, screenHeight;
    float r, g, b;
};

typedef struct Gpu_SpritePass Gpu_SpritePass;
struct Gpu_SpritePass
{
    Gpu_Sprite *sprites;
    int spriteCount;
    Gpu_Handle texture;
};

void Gpu_Startup(void);
void Gpu_Shutdown(void);
void Gpu_Present(void);
void Gpu_Clear(float r, float g, float b);
void Gpu_SubmitSprites(Gpu_SpritePass pass);

Gpu_Handle Gpu_CreateTexture(int width, int height, void *data);
void Gpu_FreeTexture(Gpu_Handle texture);

#endif // GAMES_GPU_H
