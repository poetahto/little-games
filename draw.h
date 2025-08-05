#ifndef GAMES_DRAW_H
#define GAMES_DRAW_H

#define DRAW_WHITE ((Draw_Color) { .r = 1, .g = 1, .b = 1 })
#define DRAW_RED ((Draw_Color) { .r = 1 })
#define DRAW_GREEN ((Draw_Color) { .g = 1 })
#define DRAW_BLUE ((Draw_Color) { .b = 1 })
#define DRAW_YELLOW ((Draw_Color) { .r = 1, .g = 1 })

typedef struct Draw_Color Draw_Color;
struct Draw_Color
{
    float r, g, b;
};

void Draw_BeginFrame();
void Draw_EndFrame();
void Draw_Text(int x, int y, const char *format, ...);
void Draw_Rectangle(int x, int y, int w, int h, Draw_Color color);
void Draw_Line(int x1, int y1, int x2, int y2);
void Draw_Grid(int spacing);

#endif // GAMES_DRAW_H
