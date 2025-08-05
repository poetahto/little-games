#ifndef GAMES_OS_H
#define GAMES_OS_H

#include "core.h"

typedef enum OS_EventType OS_EventType;
enum OS_EventType
{
    OS_EVENT_NULL,
    OS_EVENT_QUIT,
    OS_EVENT_KEY_DOWN,
};

typedef enum OS_KeyCode OS_KeyCode;
enum OS_KeyCode
{
    OS_KEY_NULL,
    OS_KEY_UP,
    OS_KEY_DOWN,
    OS_KEY_LEFT,
    OS_KEY_RIGHT,
    OS_KEY_ESCAPE,
};

typedef struct OS_Event OS_Event;
struct OS_Event
{
    OS_EventType type;
    OS_KeyCode key;
};

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void OS_InitWindow(int width, int height);
void OS_FreeWindow();
bool OS_PumpEvents(OS_Event *event);
void OS_Sleep(int milliseconds);
int OS_Timestamp();
void OS_Log(const char *message, ...);

void OS_RenderRect(int x, int y, int w, int h);
void OS_RenderClear();

#endif // GAMES_OS_H
