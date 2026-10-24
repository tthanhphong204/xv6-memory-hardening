#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int stack_var = 0;              // Biến này được cấp phát trên Stack
    void* heap_var = malloc(16);    // Biến này được cấp phát trên Heap

    printf("Stack address: %p\n", &stack_var);
    printf("Heap address: %p\n", heap_var);

    free(heap_var);
    exit(0);
}