#include "os.h"
#include "core.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <objbase.h>
#include <stdio.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "kernel32")
#pragma comment(lib, "bcrypt")
#pragma comment(lib, "ole32")

static Os_KeyCode Os_GetKeyCode(WPARAM wParam);
static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static Os_WindowEvent *gOsEvent;
static bool gOsEventWasProcessed;
static HWND gOsHwnd;

int main()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
    assert(hr == S_OK);
    Entrypoint();
    return 0;
}

void Os_Sleep(int milliseconds)
{
    Sleep(milliseconds);
}

void Os_Random(void *buffer, int bufferLength)
{
    BCryptGenRandom(NULL, buffer, bufferLength, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}

void Os_Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    putchar('\n');
    va_end(args);
}

void Os_CreateWindow(int width, int height)
{
    // NOTE(poe): Explicitly load default cursor to avoid spinny thing
    // on startup
    HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
    assert(cursor != NULL);

    WNDCLASS classInfo = {
        .lpfnWndProc = WndProc,
        .hCursor = cursor,
        .lpszClassName = "Game Window",
    };

    ATOM class = RegisterClass(&classInfo);
    assert(class != 0);
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    // NOTE(poe): Calculate the true window rect needed to achieve the
    // desired width + height (otherwise, titlebar is included in size)
    RECT windowRect = {
        .left = 0,
        .right = width,
        .bottom = height,
        .top = 0,
    };
    CHECK(AdjustWindowRect(&windowRect, style, FALSE));

    gOsHwnd = CreateWindowEx(
            0,                                  // ex style
            MAKEINTATOM(class),                 // class
            "Game Window",                      // title
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,   // flags
            CW_USEDEFAULT,                      // x
            CW_USEDEFAULT,                      // y
            windowRect.right - windowRect.left, // width
            windowRect.bottom - windowRect.top, // height
            NULL,                               // parent
            NULL,                               // menu
            GetModuleHandle(NULL),              // instance
            NULL);                              // param

    assert(gOsHwnd != NULL);
}

void Os_GetWindowSize(int *width, int *height)
{
    RECT rect;
    CHECK(GetClientRect(gOsHwnd, &rect));
    if (width) *width = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
}

void Os_FreeWindow()
{
    CHECK(DestroyWindow(gOsHwnd));
}

bool Os_PumpWindowEvents(Os_WindowEvent *event)
{
    gOsEvent = event;
    gOsEventWasProcessed = false;
    MSG msg;

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return gOsEventWasProcessed;
}

void * Os_GetNativeWindowHandle()
{
    return gOsHwnd;
}

static Os_KeyCode Os_GetKeyCode(WPARAM wParam)
{
    switch (wParam)
    {
        case VK_ESCAPE: return OS_KEY_ESCAPE;
        case VK_DOWN: return OS_KEY_DOWN;
        case VK_UP: return OS_KEY_UP;
        case VK_LEFT: return OS_KEY_LEFT;
        case VK_RIGHT: return OS_KEY_RIGHT;
        default: return OS_KEY_NULL;
    }
}

static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
        case WM_KEYDOWN:
        {
            bool justPressed = (lParam & (1 << 30)) == 0;

            if (justPressed)
            {
                gOsEvent->type = OS_EVENT_KEY_DOWN;
                gOsEvent->key = Os_GetKeyCode(wParam);
                gOsEventWasProcessed = true;
            }

            break;
        }
        case WM_KEYUP:
        {
            gOsEvent->type = OS_EVENT_KEY_UP;
            gOsEvent->key = Os_GetKeyCode(wParam);
            gOsEventWasProcessed = true;
            break;
        }
        case WM_DESTROY: 
        {
            // NOTE(poe): Do nothing for now...
            break;
        }
        default: 
        {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return 0;
}
