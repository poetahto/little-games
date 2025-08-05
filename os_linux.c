#include "os.h"
#include "core.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/random.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

typedef struct Os_Context Os_Context;
struct Os_Context
{
    Display *display;
    Window window;
    int screen;
    GC context;
    Os_Size size;
};

static Os_Context sOsContext;

static Os_Size Os_QueryWindowSize(Display *display, Window window);
static Os_KeyCode Os_GetKeyCode(XEvent *event);

int main()
{
    Entrypoint();
    return 0;
}

// TODO(poe): Better error checking?
void Os_InitWindow(int width, int height)
{
    Display *display = XOpenDisplay(NULL);

    if (!display)
    {
        Os_Log("Failed to open connection to X11 display server");
        exit(1);
    }

    int screen = DefaultScreen(display);

    long eventMask = 0;
    eventMask |= KeyPressMask;
    eventMask |= VisibilityChangeMask;
    eventMask |= StructureNotifyMask;
    eventMask |= ExposureMask;

    XSetWindowAttributes attributes = 
    {
        .background_pixel = BlackPixel(display, screen),
        .event_mask = eventMask,
    };

    Window window = XCreateWindow(
        display,                        // display
        RootWindow(display, screen),    // parent window
        0, 0,                           // position
        width, height,                  // size
        0,                              // border width
        CopyFromParent,                 // depth
        InputOutput,                    // class
        CopyFromParent,                 // visual
        CWBackPixel | CWEventMask,      // which attributes are being set 
        &attributes);

    GC context = XCreateGC(display, window, 0, NULL);
    XStoreName(display, window, "Game Window");
    XMapWindow(display, window);

    sOsContext = (Os_Context)
    {
        .window = window,
        .screen = screen,
        .display = display,
        .context = context,
        .size = Os_QueryWindowSize(display, window),
    };
}

void Os_FreeWindow()
{
    Display *display = sOsContext.display;
    Window window = sOsContext.window;

    XDestroyWindow(display, window);
    if (display) XCloseDisplay(display);
}

Os_Size Os_GetWindowSize()
{
    return sOsContext.size;
}

bool Os_PumpEvents(Os_Event *result)
{
    Display *display = sOsContext.display;

    if (!XPending(display))
        return false;

    XEvent event;
    XNextEvent(display, &event);

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
            sOsContext.size = (Os_Size) 
            {
                .width = event.xconfigure.width,
                .height = event.xconfigure.height,
            };
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
    Window window = sOsContext.window;
    Display *display = sOsContext.display;

    XClearWindow(display, window);
}

void Os_RenderSetColor(float r, float g, float b)
{
    Display *display = sOsContext.display;
    GC context = sOsContext.context;

    int ir = 255 * r;
    int ig = 255 * g;
    int ib = 255 * b;
    unsigned long color = ib + (ig << 8) + (ir << 16);
    XSetForeground(display, context, color);
}

void Os_RenderRect(int x, int y, int w, int h)
{
    Window window = sOsContext.window;
    Display *display = sOsContext.display;
    GC context = sOsContext.context;

    XFillRectangle(display, window, context, x, y, w, h); 
}

void Os_RenderText(int x, int y, const char *value, int valueLength)
{
    Window window = sOsContext.window;
    Display *display = sOsContext.display;
    GC context = sOsContext.context;

    XDrawString(display, window, context, x, y, value, valueLength); 
}

void Os_RenderLine(int x1, int y1, int x2, int y2)
{
    Window window = sOsContext.window;
    Display *display = sOsContext.display;
    GC context = sOsContext.context;

    XDrawLine(display, window, context, x1, y1, x2, y2);
}

static Os_Size Os_QueryWindowSize(Display *display, Window window)
{
    Window root;
    int x, y;
    unsigned int w, h, bw, d;
    XGetGeometry(display, window, &root, &x, &y, &w, &h, &bw, &d); 
    return (Os_Size) { .width = w, .height = h };
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
