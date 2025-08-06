#ifndef GAMES_GPU_H
#define GAMES_GPU_H

typedef void *Gpu_Handle;

typedef struct Gpu_Sprite Gpu_Sprite;
struct Gpu_Sprite
{
    float atlasX, atlasY, atlasWidth, atlasHeight;
    float screenX, screenY, screenWidth, screenHeight;
};

typedef struct Gpu_SpritePass Gpu_SpritePass;
struct Gpu_SpritePass
{
    Gpu_Sprite *sprites;
    int spriteCount;
    Gpu_Handle textureAtlas;
    float cameraX, cameraY, cameraTilt, cameraZoom;
};

void Gpu_Startup(void);
void Gpu_Shutdown(void);
void Gpu_Present(void);
void Gpu_Clear(float r, float g, float b);
void Gpu_SubmitSprites(Gpu_SpritePass pass);

#endif // GAMES_GPU_H
