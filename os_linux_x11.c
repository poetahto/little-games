#include "os.h"
#include "profile.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

static Display *gX11Display;
static Window gX11Window;
static int gX11Screen;
static int gX11Width;
static int gX11Height;

static Os_KeyCode Os_GetKeyCode(XEvent *event);

void Os_CreateWindow(int width, int height)
{
    Os_Log("Initializing X11");

    PROFILE("Open display")
    {
        gX11Display = XOpenDisplay(NULL);

        if (!gX11Display)
        {
            Os_Log("Failed to open connection to X11 display server");
            exit(1);
        }

        gX11Screen = DefaultScreen(gX11Display);
    }

    PROFILE("Create window")
    {
        XSetWindowAttributes attributes = {
            .background_pixel = BlackPixel(gX11Display, gX11Screen),
            .event_mask = 0 
                | KeyPressMask 
                | KeyReleaseMask
                | VisibilityChangeMask 
                | StructureNotifyMask 
                | ExposureMask
                ,
        };

        gX11Window = XCreateWindow(
            gX11Display,                         // display
            RootWindow(gX11Display, gX11Screen), // parent window
            0, 0,                                // position
            width, height,                       // size
            0,                                   // border width
            CopyFromParent,                      // depth
            InputOutput,                         // class
            CopyFromParent,                      // visual
            CWBackPixel | CWEventMask,           // the attributes being set 
            &attributes);

        XStoreName(gX11Display, gX11Window, "Game Window");
        XMapWindow(gX11Display, gX11Window);
    }

    // Figure out our _actual_ size, in case the window manager decided
    // to give us something we didn't request.
    PROFILE("Get window size")
    {
        Window r;
        int x, y;
        unsigned int w, h, bw, d;
        XGetGeometry(gX11Display, gX11Window, &r, &x, &y, &w, &h, &bw, &d); 
        gX11Width = w;
        gX11Height = h;
    }
}

void Os_FreeWindow()
{
    XDestroyWindow(gX11Display, gX11Window);
    XCloseDisplay(gX11Display);
}

void Os_GetWindowSize(int *width, int *height)
{
    if (width) *width = gX11Width;
    if (height) *height = gX11Height;
}

void * Os_GetNativeWindowHandle()
{
    return (void *)gX11Window;
}

bool Os_PumpWindowEvents(Os_WindowEvent *currentEvent)
{
    if (!XPending(gX11Display))
        return false;

    XEvent xevent;
    XNextEvent(gX11Display, &xevent);

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
        case KeyRelease:
        {
            currentEvent->type = OS_EVENT_KEY_UP;
            currentEvent->key = Os_GetKeyCode(&xevent);
            break;
        }
        case ConfigureNotify:
        {
            gX11Width = xevent.xconfigure.width;
            gX11Height = xevent.xconfigure.height;
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
