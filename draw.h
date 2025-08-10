#ifndef GAMES_DRAW_H
#define GAMES_DRAW_H

#include "gpu.h"

#define DRAW_WHITE ((Draw_Color) { .r = 1, .g = 1, .b = 1 })
#define DRAW_RED ((Draw_Color) { .r = 1 })
#define DRAW_GREEN ((Draw_Color) { .g = 1 })
#define DRAW_BLUE ((Draw_Color) { .b = 1 })
#define DRAW_YELLOW ((Draw_Color) { .r = 1, .g = 1 })

typedef struct {
    float r, g, b;
} Draw_Color;

void Draw_BeginFrame();
void Draw_EndFrame();
void Draw_Rectangle(int x, int y, int width, int height, Draw_Color color);
void Draw_Texture(Gpu_Handle texture, int x, int y, int width, int height, Draw_Color color);
void Draw_Grid(int spacing);

#endif // GAMES_DRAW_H
