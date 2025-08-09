#include "core.h"

#include <stdlib.h>
#include <string.h>

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
    memcpy(dest, src, count);
}

void MemoryClear(void *buffer, int sizeBytes)
{
    memset(buffer, 0, sizeBytes);
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
