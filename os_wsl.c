#include "os.h"
#include "core.h"

#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

static Display *sDisplay = NULL;
static Window sWindow;
static int sScreen;
static GC sContext;

int main()
{
    Entrypoint();
    return 0;
}

// TODO(poe): Better error checking?
void OS_InitWindow(int width, int height)
{
    sDisplay = XOpenDisplay(NULL);

    if (!sDisplay)
    {
        OS_Log("Failed to open connection to X11 display server");
        exit(1);
    }

    sScreen = DefaultScreen(sDisplay);

    XSetWindowAttributes attributes = 
    {
        .background_pixel = BlackPixel(sDisplay, sScreen),
        .event_mask = KeyPressMask,
    };

    sWindow = XCreateWindow(
        sDisplay,                       // display
        RootWindow(sDisplay, sScreen),   // parent window
        0, 0,                           // position
        width, height,                  // size
        0,                              // border width
        CopyFromParent,                 // depth
        InputOutput,                    // class
        CopyFromParent,                 // visual
        CWBackPixel | CWEventMask,      // which attributes are being set 
        &attributes);

    sContext = XCreateGC(sDisplay, sWindow, 0, NULL);
    XStoreName(sDisplay, sWindow, "Game Window");
    XMapWindow(sDisplay, sWindow);
}

void OS_FreeWindow()
{
    if (sWindow) XDestroyWindow(sDisplay, sWindow);
    if (sDisplay) XCloseDisplay(sDisplay);
}

static OS_KeyCode OS_GetKeyCode(XEvent *event)
{
    switch (XLookupKeysym(&event->xkey, 0))
    {
        case XK_Up: return OS_KEY_UP; 
        case XK_Down: return OS_KEY_DOWN;
        case XK_Left: return OS_KEY_LEFT;
        case XK_Right: return OS_KEY_RIGHT;
        case XK_Escape: return OS_KEY_ESCAPE;
        default: return OS_KEY_NULL;
    }
}

bool OS_PumpEvents(OS_Event *result)
{
    if (!XPending(sDisplay))
        return false;

    XEvent event;
    XNextEvent(sDisplay, &event);

    switch (event.type)
    {
        case ClientMessage: 
        {
            result->type = OS_EVENT_QUIT; 
            break;
        }
        case KeyPress:
        {
            result->type = OS_EVENT_KEY_DOWN;
            result->key = OS_GetKeyCode(&event);
            break;
        }
    }

    return true;
}

void OS_Sleep(int milliseconds)
{
    int microseconds = milliseconds * 1000;
    usleep(microseconds);
}

int OS_Timestamp(void)
{
    return 0;
}

void OS_Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    putchar('\n');
    va_end(args);
}

// TODO(poe): Move this to a render layer

void OS_RenderClear()
{
    XClearWindow(sDisplay, sWindow);
}

void OS_RenderRect(int x, int y, int w, int h)
{
    XSetForeground(sDisplay, sContext, WhitePixel(sDisplay, sScreen));
    XFillRectangle(sDisplay, sWindow, sContext, x, y, w, h); 
}
