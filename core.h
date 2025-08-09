#ifndef GAMES_CORE_H
#define GAMES_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#define UNUSED(val) val = val
#define KB(count) (count * 1024)
#define MB(count) (count * KB(1024))
#define GB(count) (count * MB(1024))

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef struct Arena Arena;
struct Arena
{
    unsigned char *buffer;
    int capacity;
    int head;
};

u16 LfsrFibonacci(u16 *lfsr);
Arena CreateArena(int capacity);
void FreeArena(Arena arena);
void * ArenaAlloc(Arena *arena, int sizeBytes);
void ArenaRollback(Arena *arena, int count);
void ArenaReset(Arena *arena);
void * HeapAlloc(int sizeBytes);
void HeapFree(void *buffer);
void MemoryClear(void *buffer, int sizeBytes);

#endif // GAMES_CORE_H
