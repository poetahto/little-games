#ifndef GAMES_DRAW_H
#define GAMES_DRAW_H

void Draw_BeginFrame();
void Draw_EndFrame();
void Draw_Text(int x, int y, const char *format, ...);
void Draw_Rectangle(int x, int y, int w, int h);
void Draw_Line(int x1, int y1, int x2, int y2);
void Draw_Grid(int spacing);

#endif // GAMES_DRAW_H
