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

static Os_WindowEvent *s_CurrentEvent;
static bool s_WasCurrentEventProcessed;
static HWND s_OsHwnd;

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
    WNDCLASS wndClass = 
    {
        .lpfnWndProc = WndProc,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = "Game Window",
    };

    RegisterClass(&wndClass);

    s_OsHwnd = CreateWindowEx(
            0,                                  // ex style
            wndClass.lpszClassName,             // class
            "Game Window",                      // title
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,   // flags
            CW_USEDEFAULT,                      // x
            CW_USEDEFAULT,                      // y
            width,                              // width
            height,                             // height
            NULL,                               // parent
            NULL,                               // menu
            GetModuleHandle(NULL),              // instance
            NULL);                              // param
}

void Os_GetWindowSize(int *width, int *height)
{
    RECT rect;
    GetClientRect(s_OsHwnd, &rect);
    if (width) *width = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
}

void Os_FreeWindow()
{
    DestroyWindow(s_OsHwnd);
}

bool Os_PumpWindowEvents(Os_WindowEvent *event)
{
    s_CurrentEvent = event;
    MSG msg;

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return s_WasCurrentEventProcessed;
}

void * Os_GetNativeWindowHandle()
{
    return s_OsHwnd;
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
    s_WasCurrentEventProcessed = false;

    switch (message) 
    {
        case WM_KEYDOWN:
        {
            bool justPressed = (lParam & (1 << 30)) == 0;

            if (justPressed)
            {
                s_CurrentEvent->type = OS_EVENT_KEY_DOWN;
                s_CurrentEvent->key = Os_GetKeyCode(wParam);
                s_WasCurrentEventProcessed = true;
            }

            break;
        }
        case WM_KEYUP:
        {
            s_CurrentEvent->type = OS_EVENT_KEY_UP;
            s_CurrentEvent->type = Os_GetKeyCode(wParam);
            s_WasCurrentEventProcessed = true;
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
