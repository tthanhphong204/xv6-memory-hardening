#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

<<<<<<< HEAD
int main(int argc, char *argv[]) {
    int stack_var = 0;              // Biến này được cấp phát trên Stack
    void* heap_var = malloc(16);    // Biến này được cấp phát trên Heap

    printf("Stack address: %p\n", &stack_var);
    printf("Heap address: %p\n", heap_var);

    free(heap_var);
    exit(0);
=======
int main(int argc, char *argv[])
{
  // Khai báo biến cục bộ để lấy địa chỉ Stack
  int local_var = 0;
  
  // In ra địa chỉ dưới định dạng chuẩn để dễ dàng parse/đọc log sau này
  printf("ENTROPY_DUMP: stack_addr=%p\n", (void*)&local_var);
  
  exit(0);
>>>>>>> sondev
}