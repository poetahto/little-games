#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#define UNUSED(val) val = val

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

u16 LfsrFibonacci(u16 *lfsr);
