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

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void Os_InitWindow(int width, int height);
void Os_FreeWindow();
bool Os_PumpEvents(Os_Event *event);
void Os_Sleep(int milliseconds);
void Os_Random(void *buffer, int bufferLength);
void Os_Log(const char *message, ...);

void Os_RenderRect(int x, int y, int w, int h);
void Os_RenderClear();

#endif // GAMES_OS_H
