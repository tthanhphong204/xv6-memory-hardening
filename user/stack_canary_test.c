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

__attribute__((noinline))
void overflow_test(void)
{
    volatile char buffer[16];

    printf("Starting buffer overflow test...\n");

    // Ghi dữ liệu vượt giới hạn buffer.
    for (int i = 0; i < 64; i++) {
        buffer[i] = 'A';
    }

    // Đọc dữ liệu để tránh cảnh báo unused.
    printf("Buffer value: %c\n", buffer[0]);
    printf("WARNING: overflow was not detected!\n");
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "overflow") == 0) {
        overflow_test();
        exit(0);
    }

    normal_test();
    printf("Stack canary normal test passed.\n");
    exit(0);
}