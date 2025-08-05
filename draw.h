#ifndef GAMES_DRAW_H
#define GAMES_DRAW_H

void Draw_BeginFrame();
void Draw_EndFrame();
void Draw_Text(int x, int y, const char *format, ...);
void Draw_Rectangle(int x, int y, int w, int h);

#endif // GAMES_DRAW_H
