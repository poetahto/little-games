#include "core.h"

u16 LfsrFibonacci(u16 *lfsr)
{
    u16 bit = ((*lfsr >> 0) ^ (*lfsr >> 2) ^ (*lfsr >> 3) ^ (*lfsr >> 5)) & 1;
    *lfsr = (*lfsr >> 1) | (bit << 15);
    return *lfsr;
}

void * ArenaAlloc(Arena *arena, int sizeBytes)
{
    assert(arena->head + sizeBytes < arena->capacity);
    void *buffer = arena->buffer + arena->head;
    arena->head += sizeBytes;
    return buffer;
}

void ArenaRollback(Arena *arena, int count)
{
    assert(arena->head - count >= 0);
    arena->head -= count;
}

void ArenaReset(Arena *arena)
{
    arena->head = 0;
}

void MemoryCopy(void *dest, const void *src, int count)
{
    u8 *bd = dest;
    const u8 *bs = src;

    for (int i = 0; i < count; i++)
        bd[i] = bs[i];
}

void MemoryClear(void *buffer, int sizeBytes)
{
    u8 *b = buffer;

    for (int i = 0; i < sizeBytes; i++)
        b[i] = 0;
}

Image ImageLoadBmp(const u8 *data, Arena *arena)
{
    // Quick and dirty BMP loading.
    // https://en.wikipedia.org/wiki/BMP_file_format#DIBs_in_memory

    int width = *(int *)(data + 0x12);
    int height = *(int *)(data + 0x16);
    int sizeBytes = width * height * 4;
    u8 *pixels = ArenaAlloc(arena, sizeBytes);
    MemoryCopy(pixels, data + *(int *)(data + 0x0A), sizeBytes);

    return (Image) {
        .width = width,
        .height = height,
        .pixels = pixels,
    };
}

Float2 CreateFloat2(float x, float y)
{
    return (Float2) { .x = x, .y = y };
}

Float2 AddFloat2(Float2 a, Float2 b)
{
    return CreateFloat2(a.x + b.x, a.y + b.y);
}

Float2 SubtractFloat2(Float2 a, Float2 b)
{
    return CreateFloat2(a.x - b.x, a.y - b.y);
}

Float2 ScaleFloat2(Float2 a, Float2 b)
{
    return CreateFloat2(a.x * b.x, a.y * b.y);
}
