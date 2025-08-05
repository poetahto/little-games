#include "core.h"
#include <stdlib.h>
#include <string.h>

u16 LfsrFibonacci(u16 *lfsr)
{
    u16 bit = ((*lfsr >> 0) ^ (*lfsr >> 2) ^ (*lfsr >> 3) ^ (*lfsr >> 5)) & 1;
    *lfsr = (*lfsr >> 1) | (bit << 15);
    return *lfsr;
}

Arena CreateArena(int capacity)
{
    return (Arena)
    {
        .buffer = HeapAlloc(capacity),
        .capacity = capacity,
    };
}

void FreeArena(Arena arena)
{
    HeapFree(arena.buffer);
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

void * HeapAlloc(int sizeBytes)
{
    return malloc(sizeBytes);
}

void HeapFree(void *buffer)
{
    free(buffer);
}

void MemoryClear(void *buffer, int sizeBytes)
{
    memset(buffer, 0, sizeBytes);
}
