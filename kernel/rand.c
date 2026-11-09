// kernel/rand.c
static unsigned long long current_seed = 123456789ULL;

void srand(unsigned long long seed) {
    current_seed = seed;
}

unsigned long long rand() {
    const unsigned long long a = 6364136223846793005ULL;
    const unsigned long long c = 1442695040888963407ULL;
    current_seed = current_seed * a + c;
    return current_seed;
}