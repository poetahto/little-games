#include "os.h"
#include "core.h"

#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>

static Display *sDisplay = NULL;
static Window sWindow;
static int sScreen;
static GC sContext;

int main()
{
    Entrypoint();
    return 0;
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

// TODO(poe): Better error checking?
void OS_InitWindow(int width, int height)
{
    UNUSED(width);
    UNUSED(height);

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
        .event_mask = ExposureMask | KeyPressMask,
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
        case Expose: 
        {
            XSetForeground(sDisplay, sContext, WhitePixel(sDisplay, sScreen));
            XFillRectangle(sDisplay, sWindow, sContext, 10, 10, 10, 10); 
            break;
        }
        case KeyPress:
        {
            if (event.xkey.keycode == 9) 
                result->type = OS_EVENT_QUIT; 

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
