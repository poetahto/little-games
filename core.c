#include <stdint.h>

unsigned lfsr_fibonacci(uint16_t start_state)
{
    uint16_t lfsr = start_state;
    uint16_t bit;
    unsigned period = 0;

    do
    {
        bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
        lfsr = (lfsr >> 1) | (bit << 15);
        ++period;
    } while (lfsr != start_state);

    return period;
}