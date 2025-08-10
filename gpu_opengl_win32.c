#define GLAD_WGL_IMPLEMENTATION
#include "extern/wgl.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "opengl32")
#pragma comment(lib, "gdi32")

static HWND gWglHwnd;
static HDC gWglDc;
static HGLRC gWglContext;

void Gpu_StartupOpenGL()
{
    PIXELFORMATDESCRIPTOR pixelFormatInfo = {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 32,
        .cDepthBits = 24,
        .cStencilBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
    };

    int attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    int pixelFormat;
    HGLRC tempContext; 

    CHECK(gWglHwnd = Os_GetNativeWindowHandle());
    CHECK(gWglDc = GetDC(gWglHwnd));
    CHECK(pixelFormat = ChoosePixelFormat(gWglDc, &pixelFormatInfo));
    CHECK(SetPixelFormat(gWglDc, pixelFormat, &pixelFormatInfo));
    CHECK(tempContext = wglCreateContext(gWglDc));
    CHECK(wglMakeCurrent(gWglDc, tempContext));
    CHECK(gladLoaderLoadWGL(gWglDc));
    CHECK(gWglContext = wglCreateContextAttribsARB(gWglDc, NULL, attributes));
    CHECK(wglMakeCurrent(NULL, NULL));
    CHECK(wglDeleteContext(tempContext));
    CHECK(wglMakeCurrent(gWglDc, gWglContext));
    CHECK(gladLoaderLoadGL());
}

void Gpu_ShutdownOpenGL()
{
    wglDeleteContext(gWglContext);
    ReleaseDC(gWglHwnd, gWglDc);
    // Do not cleanup the hwnd, since we do not own it (the os layer does)
}

void Gpu_Present()
{
    SwapBuffers(gWglDc);
}
