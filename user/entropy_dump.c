#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  // Khai báo biến cục bộ để lấy địa chỉ Stack
  int local_var = 0;
  
  // In ra địa chỉ dưới định dạng chuẩn để dễ dàng parse/đọc log sau này
  printf("ENTROPY_DUMP: stack_addr=%p\n", (void*)&local_var);
  
  exit(0);
}