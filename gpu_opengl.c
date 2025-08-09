///////////////////////////////////
// Core cross-platform OpenGL logic
//

#include "os.h"
#include "gpu.h"
#include "profile.h"

#define GLAD_GL_IMPLEMENTATION
#include "extern/gl.h"
#undef GLAD_GL_IMPLEMENTATION

#define GPU_MAX_SPRITES 256
#define GPU_MAX_TEXTURES 32

typedef enum {
    GPU_BUFFER_SPRITE_VERTEX,
    GPU_BUFFER_SPRITE_INDEX,
    GPU_BUFFER_COUNT,
} Gpu_Buffer;

typedef enum {
    GPU_VAO_SPRITE,
    GPU_VAO_COUNT,
} Gpu_Vao;

typedef enum {
    GPU_PROGRAM_SPRITE,
    GPU_PROGRAM_COUNT,
} Gpu_Program;

typedef struct {
    bool isUsed;
    int width, height;
    GLuint handle;
} Gpu_Texture;

typedef struct {
    float clipPosition[2];
    float texcoords[2];
    float tint[3];
} Gpu_SpriteVertex;

typedef struct { 
    GLuint data[GPU_BUFFER_COUNT]; 
} Gpu_BufferArray;

typedef struct { 
    GLuint data[GPU_VAO_COUNT]; 
} Gpu_VaoArray;

typedef struct { 
    GLuint data[GPU_PROGRAM_COUNT]; 
} Gpu_ProgramArray;

typedef struct {
    Gpu_BufferArray bufferArray;
    Gpu_VaoArray vaoArray;
    Gpu_ProgramArray programArray;
    Gpu_Texture textures[GPU_MAX_TEXTURES];
} Gpu_Context;

static Gpu_Context s_GpuContext;

static void Gpu_LogDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userData);
static GLuint Gpu_CreateProgram(const char *vsSource, const char *fsSource);

static void Gpu_CoreStartup(void)
{
#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Gpu_LogDebugMessage, NULL);
#endif

    Gpu_BufferArray bufferArray;
    Gpu_VaoArray vaoArray;
    Gpu_ProgramArray programArray;

    glCreateBuffers(ARR_LEN(bufferArray.data), bufferArray.data);
    glCreateVertexArrays(ARR_LEN(vaoArray.data), vaoArray.data);

    // Create sprite render state
    {
        GLuint vbo = bufferArray.data[GPU_BUFFER_SPRITE_VERTEX];
        GLuint ebo = bufferArray.data[GPU_BUFFER_SPRITE_INDEX];
        GLuint vao = vaoArray.data[GPU_VAO_SPRITE];

        const char *vsSource = 
            "#version 420\n"
            "layout (location = 0) in vec2 clipPosition;\n"
            "layout (location = 1) in vec2 texcoords;\n"
            "layout (location = 2) in vec3 tint;\n"
            "out vec2 fragTexcoords;\n"
            "out vec3 fragTint;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(clipPosition, 1.0, 1.0);\n"
            "    fragTexcoords = texcoords;\n"
            "    fragTint = tint;\n"
            "}\n";

        const char *fsSource =
            "#version 420\n"
            "in vec2 fragTexcoords;\n"
            "in vec3 fragTint;\n"
            "out vec4 outColor;\n"
            "uniform sampler2D spriteTexture;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(spriteTexture, fragTexcoords) * vec4(fragTint, 1.0);\n"
            "}\n";

        GLuint program = Gpu_CreateProgram(vsSource, fsSource);
        programArray.data[GPU_PROGRAM_SPRITE] = program;

        glNamedBufferStorage(vbo, sizeof(Gpu_SpriteVertex) * GPU_MAX_SPRITES * 4, NULL, GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferStorage(ebo, sizeof(u32) * GPU_MAX_SPRITES * 6, NULL, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Gpu_SpriteVertex));
        glVertexArrayElementBuffer(vao, ebo);

        glEnableVertexArrayAttrib(vao, 0);
        glEnableVertexArrayAttrib(vao, 1);
        glEnableVertexArrayAttrib(vao, 2);

        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, clipPosition));
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, texcoords));
        glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, tint));

        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribBinding(vao, 2, 0);
    }

    s_GpuContext = (Gpu_Context) 
    {
        .bufferArray = bufferArray,
        .vaoArray = vaoArray,
        .programArray = programArray,
    };
}

static void Gpu_CoreShutdown(void)
{
    Gpu_Context ctx = s_GpuContext;

    glDeleteVertexArrays(ARR_LEN(ctx.vaoArray.data), ctx.vaoArray.data);
    glDeleteBuffers(ARR_LEN(ctx.bufferArray.data), ctx.bufferArray.data);

    for (u32 i = 0; i < ARR_LEN(ctx.programArray.data); i++)
        glDeleteProgram(ctx.programArray.data[i]);
}

void Gpu_Clear(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

static Gpu_SpriteVertex Gpu_GetSpriteVertex(Gpu_Sprite sprite, Gpu_Texture texture, Float2 offset)
{
    int width, height; Os_GetWindowSize(&width, &height);
    Float2 center = CreateFloat2(sprite.screenX, sprite.screenY);
    Float2 halfExtents = CreateFloat2(sprite.screenWidth * 0.5f, sprite.screenHeight * 0.5f);
    Float2 halfOffset = ScaleFloat2(halfExtents, offset);
    Float2 position = AddFloat2(center, halfOffset);
    Float2 clipPosition = CreateFloat2(position.x / width * 2 - 1, position.y / height * 2 - 1);

    Float2 textureTopRight = CreateFloat2(sprite.textureX, sprite.textureY);
    Float2 textureHalfExtents = CreateFloat2(sprite.textureWidth * 0.5f, sprite.textureHeight * 0.5f);
    Float2 textureCenter = CreateFloat2(textureTopRight.x + textureHalfExtents.x, textureTopRight.y + textureHalfExtents.y);
    Float2 textureHalfOffset = ScaleFloat2(textureHalfExtents, offset);
    Float2 texturePosition = AddFloat2(textureCenter, textureHalfOffset);
    Float2 texcoords = CreateFloat2(texturePosition.x / texture.width, texturePosition.y / texture.height);

    return (Gpu_SpriteVertex)
    {
        .clipPosition = { clipPosition.x, clipPosition.y },
        .texcoords = { texcoords.x, texcoords.y },
        .tint = { sprite.r, sprite.g, sprite.b },
    };
}

void Gpu_SubmitSprites(Gpu_SpritePass pass)
{
    Gpu_Context ctx = s_GpuContext;
    Gpu_SpriteVertex vertices[GPU_MAX_SPRITES * 4];
    u32 indices[GPU_MAX_SPRITES * 6];
    Gpu_Texture texture = s_GpuContext.textures[pass.texture.dataUint];

    for (int i = 0; i < pass.spriteCount; i++)
    {
        Gpu_Sprite sprite = pass.sprites[i];

        Float2 bottomLeft = CreateFloat2(-1, -1);
        Float2 topLeft = CreateFloat2(-1, 1);
        Float2 topRight = CreateFloat2(1, 1);
        Float2 bottomRight = CreateFloat2(1, -1);

        int vi = i * 4;
        vertices[vi + 0] = Gpu_GetSpriteVertex(sprite, texture, bottomLeft);
        vertices[vi + 1] = Gpu_GetSpriteVertex(sprite, texture, topLeft); 
        vertices[vi + 2] = Gpu_GetSpriteVertex(sprite, texture, topRight); 
        vertices[vi + 3] = Gpu_GetSpriteVertex(sprite, texture, bottomRight); 

        int ei = i * 6;
        indices[ei + 0] = vi + 0;
        indices[ei + 1] = vi + 1;
        indices[ei + 2] = vi + 2;
        indices[ei + 3] = vi + 2;
        indices[ei + 4] = vi + 3;
        indices[ei + 5] = vi + 0;
    }

    glNamedBufferSubData(ctx.bufferArray.data[GPU_BUFFER_SPRITE_VERTEX], 0, sizeof(vertices), vertices);
    glNamedBufferSubData(ctx.bufferArray.data[GPU_BUFFER_SPRITE_INDEX], 0, sizeof(indices), indices);
    glUseProgram(ctx.programArray.data[GPU_PROGRAM_SPRITE]);
    glBindTextureUnit(0, texture.handle);
    glBindVertexArray(ctx.vaoArray.data[GPU_VAO_SPRITE]);
    glDrawElements(GL_TRIANGLES, pass.spriteCount * 6, GL_UNSIGNED_INT, 0);
}

Gpu_Handle Gpu_CreateTexture(int width, int height, void *data)
{
    for (u32 i = 0; i < ARR_LEN(s_GpuContext.textures); i++)
    {
        if (!s_GpuContext.textures[i].isUsed)
        {
            GLuint handle;
            glCreateTextures(GL_TEXTURE_2D, 1, &handle);
            glTextureStorage2D(handle, 1, GL_RGBA8, width, height);
            glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

            s_GpuContext.textures[i] = (Gpu_Texture)
            {
                .handle = handle,
                .width = width,
                .height = height,
                .isUsed = true,
            };

            return (Gpu_Handle) { .dataUint = i };
        }
    }

    assert(false && "No available texture slots");
    return (Gpu_Handle) {0};
}

void Gpu_FreeTexture(Gpu_Handle handle)
{
    Gpu_Texture *texture = &s_GpuContext.textures[handle.dataUint];
    texture->isUsed = false;
    glDeleteTextures(1, &texture->handle);
}

static void Gpu_LogDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userData)
{
    UNUSED(length);
    UNUSED(userData);

    const char *sourceString;
    const char *typeString;
    const char *severityString;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API: sourceString = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceString = "WINDOW SYSTEM"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceString = "SHADER COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: sourceString = "THIRD PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION: sourceString = "APPLICATION"; break;
        case GL_DEBUG_SOURCE_OTHER: sourceString = "OTHER"; break;
        default: sourceString = "???"; break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: typeString = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "DEPRECATED BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeString = "UNDEFINED BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY: typeString = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: typeString = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_MARKER: typeString = "MARKER"; break;
        case GL_DEBUG_TYPE_OTHER: typeString = "OTHER"; break;
        default: typeString = "???"; break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityString = "NOTIFICATION"; break;
        case GL_DEBUG_SEVERITY_LOW: severityString = "LOW"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severityString = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_HIGH: severityString = "HIGH"; break;
        default: severityString = "???"; break;
    }

    Os_Log("%s, %s, %s, %u: %s", sourceString, typeString, severityString, id, message);
}

static GLuint Gpu_CreateProgram(const char *vsSource, const char *fsSource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vsSource, NULL);
    glShaderSource(fragmentShader, 1, &fsSource, NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

////////////////////////////////////
// Linux EGL-based OpenGL loader
//

#ifdef OS_LINUX

#define GLAD_EGL_IMPLEMENTATION
#include "extern/egl.h"
#undef GLAD_EGL_IMPLEMENTATION

static EGLDisplay s_EGLDisplay;
static EGLSurface s_EGLSurface;
static EGLContext s_EGLContext;

static void Gpu_LinuxStartup()
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

    EGLBoolean success;
    success = gladLoaderLoadEGL(NULL);
    assert(success);
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(display != EGL_NO_DISPLAY);
    success = eglInitialize(display, NULL, NULL);
    assert(success);
    success = gladLoaderLoadEGL(display);
    assert(success);
    EGLConfig config;
    EGLint configCount;
    eglChooseConfig(display, attributes, &config, 1, &configCount);
    assert(configCount >= 1);
    EGLNativeWindowType window = (EGLNativeWindowType)Os_GetNativeWindowHandle();
    EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
    assert(surface != EGL_NO_SURFACE);
    eglBindAPI(EGL_OPENGL_API);
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    assert(context != EGL_NO_CONTEXT);
    success = eglMakeCurrent(display, surface, surface, context);
    assert(success);
    success = gladLoaderLoadGL();
    assert(success);

    s_EGLDisplay = display;
    s_EGLSurface = surface;
    s_EGLContext = context;
}

static void Gpu_LinuxShutdown()
{
    eglDestroySurface(s_EGLDisplay, s_EGLSurface);
    eglDestroyContext(s_EGLDisplay, s_EGLContext);
    eglTerminate(s_EGLDisplay);
}

static void Gpu_LinuxSwapBuffers()
{
    eglSwapBuffers(s_EGLDisplay, s_EGLSurface);
}

#endif // OS_LINUX

///////////////////////////////////////
// Windows WGL-based OpenGL loader
//

#ifdef OS_WIN32

#define GLAD_WGL_IMPLEMENTATION
#include "extern/wgl.h"
#undef GLAD_WGL_IMPLEMENTATION

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "opengl32")
#pragma comment(lib, "gdi32")

static HGLRC s_Win32RenderContext;
static HDC s_Win32HDC;

static void * Gpu_Win32GetProcAddress(const char *name)
{
    void *proc = wglGetProcAddress(name);
    if (!proc)
        proc = GetProcAddress(GetModuleHandleA("opengl32.dll"), name);
    return proc;
}

static void Gpu_Win32Startup()
{
    PIXELFORMATDESCRIPTOR pfd = (PIXELFORMATDESCRIPTOR)
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int attributes[] = 
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    BOOL result;
    HWND hwnd = Os_GetNativeWindowHandle();
    assert(hwnd != NULL);
    HDC hdc = GetDC(hwnd);
    assert(hdc != NULL);
    int pixelFormat = ChoosePixelFormat(hdc, &pfd); 
    assert(pixelFormat != 0);
    result = SetPixelFormat(hdc, pixelFormat, &pfd);
    assert(result == TRUE);
    HGLRC tempHglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempHglrc);
    result = gladLoaderLoadWGL(hdc);
    assert(result);
    HGLRC hglrc = wglCreateContextAttribsARB(hdc, NULL, attributes);
    assert(hglrc != NULL);
    result = wglMakeCurrent(NULL, NULL);
    assert(result == TRUE);
    result = wglDeleteContext(tempHglrc);
    assert(result == TRUE);
    result = wglMakeCurrent(hdc, hglrc);
    assert(result == TRUE);
    result = gladLoaderLoadGL();
    assert(result);

    s_Win32RenderContext = hglrc;
    s_Win32HDC = hdc;
}

static void Gpu_Win32Shutdown()
{
    wglDeleteContext(s_Win32RenderContext);
}

static void Gpu_Win32SwapBuffers()
{
    SwapBuffers(s_Win32HDC);
}

#endif // OS_WIN32

////////////////////////////////////
// Conditionally compiled platform-dependent functions
//

void Gpu_Startup()
{
#ifdef OS_LINUX
    Gpu_LinuxStartup();
#endif 
#ifdef OS_WIN32
    Gpu_Win32Startup();
#endif
    Gpu_CoreStartup();
}

void Gpu_Shutdown()
{
    Gpu_CoreShutdown();
#ifdef OS_LINUX
    Gpu_LinuxShutdown();
#endif
#ifdef OS_WIN32
    Gpu_Win32Shutdown();
#endif
}

void Gpu_Present()
{
#ifdef OS_LINUX
    Gpu_LinuxSwapBuffers();
#endif
#ifdef OS_WIN32
    Gpu_Win32SwapBuffers();
#endif
}
