#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

static struct spinlock random_lock;
static uint64 random_state;

// Initialize the pseudo-random number generator.
void
randominit(void)
{
  uint64 seed;

  initlock(&random_lock, "random");

  // Use RISC-V timer and memory address as initial seed.
  seed = r_time() ^ (uint64)&random_state;

  // Avoid zero state.
  if (seed == 0)
    seed = 0x9e3779b97f4a7c15ULL;

  random_state = seed;
}

// Return a pseudo-random 64-bit value.
uint64
random_u64(void)
{
  uint64 x;

  acquire(&random_lock);

  x = random_state;

  // Xorshift64* algorithm.
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;

  random_state = x;

  x *= 0x2545F4914F6CDD1DULL;

  release(&random_lock);

  return x;
}
