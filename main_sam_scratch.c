#include "os.c"
#include "core.c"

void Entrypoint()
{
    u16 seed = 0xB00B;

    for (int i = 0; i < 10; i++) {
        u16 rnd = LfsrFibonacci(&seed);
        OS_Log("%u", rnd);
    }
}
