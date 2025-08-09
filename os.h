#ifndef GAMES_OS_H
#define GAMES_OS_H

#include "core.h"

typedef enum Os_WindowEventType Os_WindowEventType;
enum Os_WindowEventType
{
    OS_EVENT_NULL,
    OS_EVENT_QUIT,
    OS_EVENT_KEY_DOWN,
    OS_EVENT_KEY_UP,
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

typedef struct Os_WindowEvent Os_WindowEvent;
struct Os_WindowEvent
{
    Os_WindowEventType type;
    Os_KeyCode key;
};

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void Os_Sleep(int milliseconds);
void Os_Random(void *buffer, int bufferLength);
void Os_Log(const char *message, ...);
void * Os_HeapAlloc(int sizeBytes);
void Os_HeapFree(void *buffer);
void Os_CreateWindow(int width, int height);
void Os_GetWindowSize(int *width, int *height);
void Os_FreeWindow();
bool Os_PumpWindowEvents(Os_WindowEvent *event);
void * Os_GetNativeWindowHandle();

#endif // GAMES_OS_H
