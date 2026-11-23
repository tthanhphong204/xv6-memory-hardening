#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

static uint64 prng_state = 0x123456789ABCDEF0;

void randinit(void) {
    prng_state ^= r_time();
    if (prng_state == 0)
        prng_state = 0x123456789ABCDEF0;
}

unsigned long long rand(void) {
    prng_state ^= r_time();
    if (prng_state == 0)
        prng_state = 0x123456789ABCDEF0;

    prng_state ^= prng_state << 13;
    prng_state ^= prng_state >> 7;
    prng_state ^= prng_state << 17;
    return prng_state;
}