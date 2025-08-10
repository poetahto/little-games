#ifndef GAMES_CORE_H
#define GAMES_CORE_H

#include "extern/incbin.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>

#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define UNUSED(val) val = val
#define CHECK(expr) if (!(expr)) assert(false)
#define KB(count) (count * 1024)
#define MB(count) (count * 1024 * 1024)
#define GB(count) (count * 1024 * 1024 * 1024)

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef union {
    struct { float x, y; };
    float data[2];
} Float2;

typedef struct {
    unsigned char *buffer;
    int capacity;
    int head;
} Arena;

typedef struct {
    void *pixels;
    int width;
    int height;
} Image;

u16 LfsrFibonacci(u16 *lfsr);
void * ArenaAlloc(Arena *arena, int sizeBytes);
void ArenaRollback(Arena *arena, int count);
void ArenaReset(Arena *arena);
void MemoryCopy(void *dest, const void *src, int count);
void MemoryClear(void *buffer, int sizeBytes);
Image ImageLoadBmp(const u8 *data, Arena *arena);
Float2 CreateFloat2(float x, float y);
Float2 AddFloat2(Float2 a, Float2 b);
Float2 SubtractFloat2(Float2 a, Float2 b);
Float2 ScaleFloat2(Float2 a, Float2 b);

#endif // GAMES_CORE_H
