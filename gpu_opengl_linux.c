#define GLAD_EGL_IMPLEMENTATION
#include "extern/egl.h"

static EGLDisplay gEglDisplay;
static EGLSurface gEglSurface;
static EGLContext gEglContext;

void Gpu_StartupOpenGL()
{
    EGLint attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE,
    };

    EGLConfig config;
    EGLint configCount;
    EGLNativeWindowType window;

    CHECK(gladLoaderLoadEGL(NULL));
    CHECK(gEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY));
    CHECK(eglInitialize(gEglDisplay, NULL, NULL));
    CHECK(gladLoaderLoadEGL(gEglDisplay));
    CHECK(eglBindAPI(EGL_OPENGL_API));
    CHECK(eglChooseConfig(gEglDisplay, attributes, &config, 1, &configCount));
    CHECK(configCount >= 1);
    CHECK(gEglContext = eglCreateContext(gEglDisplay, config, EGL_NO_CONTEXT, NULL));
    CHECK(window = (EGLNativeWindowType)Os_GetNativeWindowHandle());
    CHECK(gEglSurface = eglCreateWindowSurface(gEglDisplay, config, window, NULL));
    CHECK(eglMakeCurrent(gEglDisplay, gEglSurface, gEglSurface, gEglContext));
}

void Gpu_ShutdownOpenGL()
{
    eglDestroySurface(gEglDisplay, gEglSurface);
    eglDestroyContext(gEglDisplay, gEglContext);
    eglTerminate(gEglDisplay);
}

void Gpu_Present()
{
    eglSwapBuffers(gEglDisplay, gEglSurface);
}
