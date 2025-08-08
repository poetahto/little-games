#include "gpu.h"
#include "wm.h"
#include "profile.h"

#include "extern/glad/glad.c"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <GL/glext.h>

#define GPU_MAX_SPRITES 256
#define GPU_MAX_TEXTURES 32

typedef enum Gpu_Buffer Gpu_Buffer;
enum Gpu_Buffer
{
    GPU_BUFFER_SPRITE_VERTEX,
    GPU_BUFFER_SPRITE_INDEX,
    GPU_BUFFER_COUNT,
};

typedef enum Gpu_Vao Gpu_Vao;
enum Gpu_Vao
{
    GPU_VAO_SPRITE,
    GPU_VAO_COUNT,
};

typedef enum Gpu_Program Gpu_Program;
enum Gpu_Program
{
    GPU_PROGRAM_SPRITE,
    GPU_PROGRAM_COUNT,
};

typedef struct Gpu_Texture Gpu_Texture;
struct Gpu_Texture
{
    bool isUsed;
    int width, height;
    GLuint handle;
};

typedef struct Gpu_SpriteVertex Gpu_SpriteVertex;
struct Gpu_SpriteVertex
{
    float clipPosition[2];
    float texcoords[2];
    float tint[3];
};

typedef struct Gpu_BufferArray Gpu_BufferArray;
struct Gpu_BufferArray 
{ 
    GLuint data[GPU_BUFFER_COUNT]; 
};

typedef struct Gpu_VaoArray Gpu_VaoArray;
struct Gpu_VaoArray 
{ 
    GLuint data[GPU_VAO_COUNT]; 
};

typedef struct Gpu_ProgramArray Gpu_ProgramArray;
struct Gpu_ProgramArray 
{ 
    GLuint data[GPU_PROGRAM_COUNT]; 
};

typedef struct Gpu_Context Gpu_Context;
struct Gpu_Context
{
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    Gpu_BufferArray bufferArray;
    Gpu_VaoArray vaoArray;
    Gpu_ProgramArray programArray;
    Gpu_Texture textures[GPU_MAX_TEXTURES];
};

static Gpu_Context s_GpuContext;

static void Gpu_LogDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userData);
static GLuint Gpu_CreateProgram(const char *vsSource, const char *fsSource);

void Gpu_Startup(void)
{
    Os_Log("Initialzing OpenGL");

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
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(display != EGL_NO_DISPLAY);
    success = eglInitialize(display, NULL, NULL);
    assert(success);
    EGLConfig config;
    EGLint configCount;
    eglChooseConfig(display, attributes, &config, 1, &configCount);
    assert(configCount >= 1);
    eglBindAPI(EGL_OPENGL_API);
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    assert(context != EGL_NO_CONTEXT);
    EGLNativeWindowType window = (EGLNativeWindowType)Wm_GetNativeHandle();
    EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
    assert(surface != EGL_NO_SURFACE);
    success = eglMakeCurrent(display, surface, surface, context);
    assert(success);
    success = gladLoadGLLoader((GLADloadproc)eglGetProcAddress);
    assert(success);

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
            "    outColor = texture(spriteTexture, fragTexcoords);\n"
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
        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, texcoords));
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, tint));

        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribBinding(vao, 2, 0);
    }

    s_GpuContext = (Gpu_Context) 
    {
        .display = display,
        .context = context,
        .surface = surface,
        .bufferArray = bufferArray,
        .vaoArray = vaoArray,
        .programArray = programArray,
    };
}

void Gpu_Shutdown(void)
{
    Os_Log("Freeing OpenGL resources");
    Gpu_Context ctx = s_GpuContext;

    glDeleteVertexArrays(ARR_LEN(ctx.vaoArray.data), ctx.vaoArray.data);
    glDeleteBuffers(ARR_LEN(ctx.bufferArray.data), ctx.bufferArray.data);

    for (u32 i = 0; i < ARR_LEN(ctx.programArray.data); i++)
        glDeleteProgram(ctx.programArray.data[i]);

    eglDestroySurface(ctx.display, ctx.surface);
    eglDestroyContext(ctx.display, ctx.context);
    eglTerminate(ctx.display);
}

void Gpu_Present(void)
{
    eglSwapBuffers(s_GpuContext.display, s_GpuContext.surface);
}

void Gpu_Clear(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

static Gpu_SpriteVertex Gpu_GetSpriteVertex(Gpu_Sprite sprite, Gpu_Texture texture, Float2 offset)
{
    int width, height; Wm_GetWindowSize(&width, &height);
    Float2 center = CreateFloat2(sprite.screenX, sprite.screenY);
    Float2 halfExtents = CreateFloat2(sprite.screenWidth * 0.5, sprite.screenHeight * 0.5);
    Float2 halfOffset = ScaleFloat2(halfExtents, offset);
    Float2 position = AddFloat2(center, halfOffset);
    Float2 clipPosition = CreateFloat2(position.x / width * 2 - 1, position.y / height * 2 - 1);

    Float2 textureTopRight = CreateFloat2(sprite.textureX, sprite.textureY);
    Float2 textureHalfExtents = CreateFloat2(sprite.textureWidth * 0.5, sprite.textureHeight * 0.5);
    Float2 textureCenter = CreateFloat2(textureTopRight.x + textureHalfExtents.x, textureTopRight.y - textureHalfExtents.y);
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
