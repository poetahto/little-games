#ifndef GAMES_OS_H
#define GAMES_OS_H

#include "core.h"

typedef enum Os_EventType Os_EventType;
enum Os_EventType
{
    OS_EVENT_NULL,
    OS_EVENT_QUIT,
    OS_EVENT_KEY_DOWN,
};

typedef enum Os_KeyCode Os_KeyCode;
enum Os_KeyCode
{
    OS_KEY_NULL,
    OS_KEY_UP,
    OS_KEY_DOWN,
    OS_KEY_LEFT,
    OS_KEY_RIGHT,
    OS_KEY_ESCAPE,
};

typedef struct Os_Event Os_Event;
struct Os_Event
{
    Os_EventType type;
    Os_KeyCode key;
};

typedef struct Os_Size Os_Size;
struct Os_Size
{
    int width;
    int height;
};

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void Os_InitWindow(int width, int height);
void Os_FreeWindow();
Os_Size Os_GetWindowSize();
bool Os_PumpEvents(Os_Event *event);
void Os_Sleep(int milliseconds);
void Os_Random(void *buffer, int bufferLength);
void Os_Log(const char *message, ...);

void Os_RenderRect(int x, int y, int w, int h);
void Os_RenderClear();
void Os_RenderSetColor(float r, float g, float b);
void Os_RenderText(int x, int y, const char *value, int valueLength);
void Os_RenderLine(int x1, int y1, int x2, int y2);

#endif // GAMES_OS_H
