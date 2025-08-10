#include "os.h"
#include "gpu.h"
#include "profile.h"

#ifdef OS_LINUX
#include "gpu_opengl_linux.c"
#endif

#ifdef OS_WIN32
#include "gpu_opengl_win32.c"
#endif

#define GLAD_GL_IMPLEMENTATION
#include "extern/gl.h"

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
    GLuint name;
} Gpu_Texture;

typedef struct {
    float clipPosition[2];
    float texcoords[2];
    float tint[3];
} Gpu_SpriteVertex;

static GLuint gGlBuffers[GPU_BUFFER_COUNT];
static GLuint gGlVaos[GPU_VAO_COUNT];
static GLuint gGlPrograms[GPU_PROGRAM_COUNT];
static Gpu_Texture gGlTextures[GPU_MAX_TEXTURES];

INCTXT(GlSpriteVs, "resources/sprite.vert");
INCTXT(GlSpriteFs, "resources/sprite.frag");

static void Gpu_LogDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userData);
static GLuint Gpu_CreateProgram(const char *vsSource, const char *fsSource);

void Gpu_Startup(void)
{
    Gpu_StartupOpenGL();
    if (!gladLoaderLoadGL()) assert(false);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Gpu_LogDebugMessage, NULL);
#endif

    glCreateBuffers(ARR_LEN(gGlBuffers), gGlBuffers);
    glCreateVertexArrays(ARR_LEN(gGlVaos), gGlVaos);

    // Create sprite render state
    {
        GLuint vbo = gGlBuffers[GPU_BUFFER_SPRITE_VERTEX];
        GLuint ebo = gGlBuffers[GPU_BUFFER_SPRITE_INDEX];
        GLuint vao = gGlVaos[GPU_VAO_SPRITE];

        GLuint program = Gpu_CreateProgram(gGlSpriteVsData, gGlSpriteFsData);
        gGlPrograms[GPU_PROGRAM_SPRITE] = program;

        glNamedBufferStorage(vbo, sizeof(Gpu_SpriteVertex) * GPU_MAX_SPRITES * 4, NULL, GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferStorage(ebo, sizeof(u32) * GPU_MAX_SPRITES * 6, NULL, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Gpu_SpriteVertex));
        glVertexArrayElementBuffer(vao, ebo);

        glEnableVertexArrayAttrib(vao, 0);
        glEnableVertexArrayAttrib(vao, 1);
        glEnableVertexArrayAttrib(vao, 2);

        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribBinding(vao, 2, 0);

        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, clipPosition));
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, texcoords));
        glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Gpu_SpriteVertex, tint));
    }
}

void Gpu_Shutdown(void)
{
    glDeleteVertexArrays(ARR_LEN(gGlVaos), gGlVaos);
    glDeleteBuffers(ARR_LEN(gGlBuffers), gGlBuffers);

    for (u32 i = 0; i < ARR_LEN(gGlPrograms); i++)
        glDeleteProgram(gGlPrograms[i]);

    Gpu_ShutdownOpenGL();
}

void Gpu_Clear(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

static Gpu_SpriteVertex Gpu_GetSpriteVertex(Gpu_Sprite sprite, Gpu_Handle texture, Float2 offset)
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
    Float2 texcoords = CreateFloat2(texturePosition.x / gGlTextures[texture].width, texturePosition.y / gGlTextures[texture].height);

    return (Gpu_SpriteVertex)
    {
        .clipPosition = { clipPosition.x, clipPosition.y },
        .texcoords = { texcoords.x, texcoords.y },
        .tint = { sprite.r, sprite.g, sprite.b },
    };
}

void Gpu_SubmitSprites(Gpu_SpritePass pass)
{
    Gpu_SpriteVertex vertices[GPU_MAX_SPRITES * 4];
    u32 indices[GPU_MAX_SPRITES * 6];

    for (int i = 0; i < pass.spriteCount; i++)
    {
        Gpu_Sprite sprite = pass.sprites[i];

        Float2 bottomLeft = CreateFloat2(-1, -1);
        Float2 topLeft = CreateFloat2(-1, 1);
        Float2 topRight = CreateFloat2(1, 1);
        Float2 bottomRight = CreateFloat2(1, -1);

        int vi = i * 4;
        vertices[vi + 0] = Gpu_GetSpriteVertex(sprite, pass.texture, bottomLeft);
        vertices[vi + 1] = Gpu_GetSpriteVertex(sprite, pass.texture, topLeft); 
        vertices[vi + 2] = Gpu_GetSpriteVertex(sprite, pass.texture, topRight); 
        vertices[vi + 3] = Gpu_GetSpriteVertex(sprite, pass.texture, bottomRight); 

        int ei = i * 6;
        indices[ei + 0] = vi + 0;
        indices[ei + 1] = vi + 1;
        indices[ei + 2] = vi + 2;
        indices[ei + 3] = vi + 2;
        indices[ei + 4] = vi + 3;
        indices[ei + 5] = vi + 0;
    }

    glNamedBufferSubData(gGlBuffers[GPU_BUFFER_SPRITE_VERTEX], 0, sizeof(vertices), vertices);
    glNamedBufferSubData(gGlBuffers[GPU_BUFFER_SPRITE_INDEX], 0, sizeof(indices), indices);
    glUseProgram(gGlPrograms[GPU_PROGRAM_SPRITE]);
    glBindTextureUnit(0, gGlTextures[pass.texture].name);
    glBindVertexArray(gGlVaos[GPU_VAO_SPRITE]);
    glDrawElements(GL_TRIANGLES, pass.spriteCount * 6, GL_UNSIGNED_INT, 0);
}

Gpu_Handle Gpu_CreateTexture(Image image)
{
    for (u32 i = 0; i < ARR_LEN(gGlTextures); i++)
    {
        if (!gGlTextures[i].isUsed)
        {
            GLuint name;
            glCreateTextures(GL_TEXTURE_2D, 1, &name);
            glTextureStorage2D(name, 1, GL_RGBA8, image.width, image.height);
            glTextureSubImage2D(name, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);

            gGlTextures[i] = (Gpu_Texture)
            {
                .name = name,
                .width = image.width,
                .height = image.height,
                .isUsed = true,
            };

            return i;
        }
    }

    assert(false && "No available texture slots");
    return -1;
}

void Gpu_FreeTexture(Gpu_Handle texture)
{
    gGlTextures[texture].isUsed = false;
    glDeleteTextures(1, &gGlTextures[texture].name);
}

void Gpu_GetTextureSize(Gpu_Handle texture, int *width, int *height)
{
    if (width) *width = gGlTextures[texture].width;
    if (height) *height = gGlTextures[texture].height;
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
