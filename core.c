#include <stdint.h>

uint16_t lfsr_fibonacci(uint16_t *lfsr)
{
    uint16_t bit = ((*lfsr >> 0) ^ (*lfsr >> 2) ^ (*lfsr >> 3) ^ (*lfsr >> 5)) & 1;
    *lfsr = (*lfsr >> 1) | (bit << 15);
    return *lfsr;
}