#ifndef GAMES_WM_H
#define GAMES_WM_H

typedef enum Wm_EventType Wm_EventType;
enum Wm_EventType
{
    WM_EVENT_NULL,
    WM_EVENT_QUIT,
    WM_EVENT_KEY_DOWN,
};

typedef enum Wm_KeyCode Wm_KeyCode;
enum Wm_KeyCode
{
    WM_KEY_NULL,
    WM_KEY_UP,
    WM_KEY_DOWN,
    WM_KEY_LEFT,
    WM_KEY_RIGHT,
    WM_KEY_ESCAPE,
};

typedef struct Wm_Event Wm_Event;
struct Wm_Event
{
    Wm_EventType type;
    Wm_KeyCode key;
};

void Wm_Startup(int width, int height);
void Wm_Shutdown();
void Wm_GetWindowSize(int *width, int *height);
bool Wm_PumpEvents(Wm_Event *event);

// Software rendering

void Wm_RenderClear();
void Wm_RenderSetColor(float r, float g, float b);
void Wm_RenderRect(int x, int y, int w, int h);
void Wm_RenderLine(int x1, int y1, int x2, int y2);
void Wm_RenderText(int x, int y, const char *value, int valueLength);

#endif // GAMES_WM_H
