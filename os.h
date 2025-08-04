#ifndef GAMES_OS_H
#define GAMES_OS_H

#include "core.h"

typedef enum OS_EventType OS_EventType;
enum OS_EventType
{
    OS_EVENT_QUIT,
};

typedef struct OS_Event OS_Event;
struct OS_Event
{
    OS_EventType type;
};

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void OS_InitWindow(int width, int height);
void OS_FreeWindow();
bool OS_PumpEvents(OS_Event *event);
void OS_Sleep(int milliseconds);
int OS_Timestamp();
void OS_Log(const char *message, ...);

#endif // GAMES_OS_H
