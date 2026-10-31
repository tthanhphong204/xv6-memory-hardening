#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noinline))
void normal_test(void)
{
    volatile char buffer[32];

    for (int i = 0; i < 32; i++) {
        buffer[i] = 'A';
    }

    printf("Normal test: buffer[0] = %c\n", buffer[0]);
    printf("Normal function completed.\n");
}

int main(int argc, char *argv[])
{
    normal_test();
    printf("Stack canary normal test passed.\n");

    if (argc > 1 && strcmp(argv[1], "overflow") == 0) {
        printf("Overflow test is not implemented yet.\n");
    }

    exit(0);
}
