<<<<<<< HEAD
<<<<<<< HEAD
// kernel/rand.c
static unsigned long long current_seed = 123456789ULL;
=======
#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
>>>>>>> hungdev

static uint64 prng_state = 0x123456789ABCDEF0;

void randinit(void) {
    prng_state ^= r_time();
    if (prng_state == 0)
        prng_state = 0x123456789ABCDEF0;
}

<<<<<<< HEAD
unsigned long long rand() {
    const unsigned long long a = 6364136223846793005ULL;
    const unsigned long long c = 1442695040888963407ULL;
    current_seed = current_seed * a + c;
    return current_seed;
=======
#include "types.h"

// Biến lưu trạng thái tiếp theo của bộ sinh số ngẫu nhiên
static unsigned long rand_next = 123456789;

// Hàm sinh số ngẫu nhiên (trả về một số nguyên)
int
rand(void)
{
  rand_next = rand_next * 1103515245 + 12345;
  return (unsigned int)(rand_next / 65536) % 32768;
}

// Hàm khởi tạo hạt giống (seed) để thay đổi chuỗi ngẫu nhiên khi cần
void
srand(unsigned int seed)
{
  rand_next = seed;
>>>>>>> sondev
=======
unsigned long long rand(void) {
    prng_state ^= r_time();
    if (prng_state == 0)
        prng_state = 0x123456789ABCDEF0;

    prng_state ^= prng_state << 13;
    prng_state ^= prng_state >> 7;
    prng_state ^= prng_state << 17;
    return prng_state;
>>>>>>> hungdev
}