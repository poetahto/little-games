#include "os.h"
#include "core.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/random.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

static Display *sDisplay = NULL;
static Window sWindow;
static int sScreen;
static GC sContext;
static Os_Size sSize;

int main()
{
    Entrypoint();
    return 0;
}

static void Os_UpdateWindowSize()
{
    Window root;
    int x, y;
    unsigned int w, h, bw, d;
    XGetGeometry(sDisplay, sWindow, &root, &x, &y, &w, &h, &bw, &d); 
    sSize.width = w;
    sSize.height = h;
}

// TODO(poe): Better error checking?
void Os_InitWindow(int width, int height)
{
    sDisplay = XOpenDisplay(NULL);

    if (!sDisplay)
    {
        Os_Log("Failed to open connection to X11 display server");
        exit(1);
    }

    sScreen = DefaultScreen(sDisplay);

    long eventMask = 0;
    eventMask |= KeyPressMask;
    eventMask |= VisibilityChangeMask;
    eventMask |= StructureNotifyMask;
    eventMask |= ExposureMask;

    XSetWindowAttributes attributes = 
    {
        .background_pixel = BlackPixel(sDisplay, sScreen),
        .event_mask = eventMask,
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

    Os_UpdateWindowSize();
}

void Os_FreeWindow()
{
    if (sWindow) XDestroyWindow(sDisplay, sWindow);
    if (sDisplay) XCloseDisplay(sDisplay);
}

Os_Size Os_GetWindowSize()
{
    return sSize;
}

static Os_KeyCode Os_GetKeyCode(XEvent *event)
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

bool Os_PumpEvents(Os_Event *result)
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
            result->key = Os_GetKeyCode(&event);
            break;
        }
        case ConfigureNotify:
        {
            Os_UpdateWindowSize();
            break;
        }
    }

    return true;
}

void Os_Sleep(int milliseconds)
{
    int microseconds = milliseconds * 1000;
    usleep(microseconds);
}

void Os_Random(void *buffer, int bufferLength)
{
   ssize_t count = getrandom(buffer, bufferLength, 0);

   // NOTE(poe): I don't entirely understand when this could fail, hence 
   // this curiosity-driven assert.
   assert(count == bufferLength);
}

void Os_Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    putchar('\n');
    va_end(args);
}

// TODO(poe): Move this to a render layer

void Os_RenderClear()
{
    XClearWindow(sDisplay, sWindow);
}

void Os_RenderRect(int x, int y, int w, int h)
{
    XSetForeground(sDisplay, sContext, WhitePixel(sDisplay, sScreen));
    XFillRectangle(sDisplay, sWindow, sContext, x, y, w, h); 
}

void Os_RenderText(int x, int y, const char *value, int valueLength)
{
    XSetForeground(sDisplay, sContext, WhitePixel(sDisplay, sScreen));
    XDrawString(sDisplay, sWindow, sContext, x, y, value, valueLength); 
}

void Os_RenderLine(int x1, int y1, int x2, int y2)
{
    XSetForeground(sDisplay, sContext, WhitePixel(sDisplay, sScreen));
    XDrawLine(sDisplay, sWindow, sContext, x1, y1, x2, y2);
}
