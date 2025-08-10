#ifndef GAMES_GPU_H
#define GAMES_GPU_H

typedef int Gpu_Handle;

typedef struct {
    float textureX, textureY, textureWidth, textureHeight;
    float screenX, screenY, screenWidth, screenHeight;
    float r, g, b;
} Gpu_Sprite;

typedef struct {
    Gpu_Sprite *sprites;
    int spriteCount;
    Gpu_Handle texture;
} Gpu_SpritePass;

void Gpu_Startup(void);
void Gpu_Shutdown(void);
void Gpu_Present(void);
void Gpu_Clear(float r, float g, float b);
void Gpu_SubmitSprites(Gpu_SpritePass pass);
Gpu_Handle Gpu_CreateTexture(int width, int height, void *data);
void Gpu_FreeTexture(Gpu_Handle texture);

#endif // GAMES_GPU_H
