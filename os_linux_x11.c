#include "os.h"
#include "profile.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

typedef struct Os_X11Context Os_X11Context;
struct Os_X11Context
{
    Display *display;
    Window window;
    int screen;
    int width;
    int height;
};

static Os_X11Context s_OsX11Context;

static Os_KeyCode Os_GetKeyCode(XEvent *event);

void Os_CreateWindow(int width, int height)
{
    Os_Log("Initializing X11");
    Os_X11Context ctx;

    PROFILE("Open display")
    {
        ctx.display = XOpenDisplay(NULL);

        if (!ctx.display)
        {
            Os_Log("Failed to open connection to X11 display server");
            exit(1);
        }

        ctx.screen = DefaultScreen(ctx.display);
    }

    PROFILE("Create window")
    {
        XSetWindowAttributes attributes = 
        {
            .background_pixel = BlackPixel(ctx.display, ctx.screen),
            .event_mask = 0 
                | KeyPressMask 
                | VisibilityChangeMask 
                | StructureNotifyMask 
                | ExposureMask
                ,
        };

        ctx.window = XCreateWindow(
            ctx.display,                         // display
            RootWindow(ctx.display, ctx.screen), // parent window
            0, 0,                                // position
            width, height,                       // size
            0,                                   // border width
            CopyFromParent,                      // depth
            InputOutput,                         // class
            CopyFromParent,                      // visual
            CWBackPixel | CWEventMask,           // the attributes being set 
            &attributes);

        XStoreName(ctx.display, ctx.window, "Game Window");
        XMapWindow(ctx.display, ctx.window);
    }

    // Figure out our _actual_ size, in case the window manager decided
    // to give us something we didn't request.
    PROFILE("Get window size")
    {
        Window r;
        int x, y;
        unsigned int w, h, bw, d;
        XGetGeometry(ctx.display, ctx.window, &r, &x, &y, &w, &h, &bw, &d); 
        ctx.width = w;
        ctx.height = h;
    }

    s_OsX11Context = ctx;
}

void Os_FreeWindow()
{
    Os_X11Context ctx = s_OsX11Context;
    XDestroyWindow(ctx.display, ctx.window);
    XCloseDisplay(ctx.display);
}

void Os_GetWindowSize(int *width, int *height)
{
    if (width) *width = s_OsX11Context.width;
    if (height) *height = s_OsX11Context.height;
}

bool Os_PumpEvents(Os_Event *currentEvent)
{
    Os_X11Context *ctx = &s_OsX11Context;

    if (!XPending(ctx->display))
        return false;

    XEvent xevent;
    XNextEvent(ctx->display, &xevent);

    switch (xevent.type)
    {
        case ClientMessage: 
        {
            currentEvent->type = OS_EVENT_QUIT; 
            break;
        }
        case KeyPress:
        {
            currentEvent->type = OS_EVENT_KEY_DOWN;
            currentEvent->key = Os_GetKeyCode(&xevent);
            break;
        }
        case ConfigureNotify:
        {
            ctx->width = xevent.xconfigure.width;
            ctx->height = xevent.xconfigure.height;
            break;
        }
    }

    return true;
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
