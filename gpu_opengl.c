#include "gpu.h"
#include "wm.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>

typedef struct Gpu_Context Gpu_Context;
struct Gpu_Context
{
    EGLDisplay display;
    EGLSurface surface;
};

static Gpu_Context s_GpuContext;

void Gpu_Startup(void)
{
    EGLint attributes[] = 
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    Gpu_Context ctx;
    EGLBoolean success;

    Os_Log("Getting EGL display");
    ctx.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(ctx.display != EGL_NO_DISPLAY);

    Os_Log("Initializing EGL");
    success = eglInitialize(ctx.display, NULL, NULL);
    assert(success);

    Os_Log("Choosing EGL config");
    EGLConfig config;
    EGLint configCount;
    eglChooseConfig(ctx.display, attributes, &config, 1, &configCount);
    assert(configCount >= 1);

    Os_Log("Binding OpenGL API");
    eglBindAPI(EGL_OPENGL_API);

    Os_Log("Creating EGL context");
    EGLContext context = eglCreateContext(ctx.display, config, EGL_NO_CONTEXT, NULL);
    assert(context != EGL_NO_CONTEXT);

    Os_Log("Creating EGL window surface");
    EGLNativeWindowType window = (EGLNativeWindowType)Wm_GetNativeHandle();
    ctx.surface = eglCreateWindowSurface(ctx.display, config, window, NULL);
    assert(ctx.surface != EGL_NO_SURFACE);

    Os_Log("Making EGL context current");
    success = eglMakeCurrent(ctx.display, ctx.surface, ctx.surface, context);
    assert(success);

    s_GpuContext = ctx;
}

void Gpu_Shutdown(void)
{
    Gpu_Context ctx = s_GpuContext;
    eglDestroySurface(ctx.display, ctx.surface);
    eglTerminate(ctx.display);
}

void Gpu_Present(void)
{
    Gpu_Context ctx = s_GpuContext;
    eglSwapBuffers(ctx.display, ctx.surface);
}

void Gpu_Clear(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Gpu_SubmitSprites(Gpu_SpritePass pass)
{
    UNUSED(pass);
}

