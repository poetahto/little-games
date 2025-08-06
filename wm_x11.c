#include "wm.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

typedef struct Wm_Context Wm_Context;
struct Wm_Context
{
    Display *display;
    Window window;
    int screen;
    GC gc;
    int width;
    int height;
};

static Wm_Context s_WmContext;

static Wm_KeyCode Wm_GetKeyCode(XEvent *event);

void Wm_Startup(int width, int height)
{
    Wm_Context ctx;

    Os_Log("Opening connection to X11 server");
    ctx.display = XOpenDisplay(NULL);

    if (!ctx.display)
    {
        Os_Log("Failed to open connection to X11 display server");
        exit(1);
    }

    Os_Log("Creating X11 window");
    ctx.screen = DefaultScreen(ctx.display);

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

    ctx.gc = XCreateGC(ctx.display, ctx.window, 0, NULL);
    XStoreName(ctx.display, ctx.window, "Game Window");
    XMapWindow(ctx.display, ctx.window);

    // Figure out our _actual_ size, in case the window manager decided
    // to give us something we didn't request.
    {
        Os_Log("Getting X11 window size");
        Window r;
        int x, y;
        unsigned int w, h, bw, d;
        XGetGeometry(ctx.display, ctx.window, &r, &x, &y, &w, &h, &bw, &d); 
        ctx.width = w;
        ctx.height = h;
    }

    s_WmContext = ctx;
}

void Wm_Shutdown()
{
    Wm_Context ctx = s_WmContext;
    XDestroyWindow(ctx.display, ctx.window);
    XCloseDisplay(ctx.display);
}

void Wm_GetWindowSize(int *width, int *height)
{
    if (width) *width = s_WmContext.width;
    if (height) *height = s_WmContext.height;
}

bool Wm_PumpEvents(Wm_Event *currentEvent)
{
    Wm_Context *ctx = &s_WmContext;

    if (!XPending(ctx->display))
        return false;

    XEvent xevent;
    XNextEvent(ctx->display, &xevent);

    switch (xevent.type)
    {
        case ClientMessage: 
        {
            currentEvent->type = WM_EVENT_QUIT; 
            break;
        }
        case KeyPress:
        {
            currentEvent->type = WM_EVENT_KEY_DOWN;
            currentEvent->key = Wm_GetKeyCode(&xevent);
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

void *Wm_GetNativeHandle(void)
{
    return (void *)s_WmContext.window;
}

static Wm_KeyCode Wm_GetKeyCode(XEvent *event)
{
    switch (XLookupKeysym(&event->xkey, 0))
    {
        case XK_Up: return WM_KEY_UP; 
        case XK_Down: return WM_KEY_DOWN;
        case XK_Left: return WM_KEY_LEFT;
        case XK_Right: return WM_KEY_RIGHT;
        case XK_Escape: return WM_KEY_ESCAPE;
        default: return WM_KEY_NULL;
    }
}
