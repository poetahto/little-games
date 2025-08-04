#include "os.c"
#include "core.c"

void Entrypoint()
{
    uint16_t seed = 0xB00B;

    for (int i = 0; i < 10; i++) {
        uint16_t rnd = lfsr_fibonacci(&seed);
        OS_Log("%u", rnd);
    }
}