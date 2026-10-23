#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Dang kiem thu loi truy cap con tro NULL ===\n");
  
  // Khởi tạo con trỏ NULL (địa chỉ 0)
  volatile int *ptr = (int *)0;
  
  // Cố gắng đọc giá trị tại địa chỉ 0
  int val = *ptr;
  
  printf("Gia tri doc duoc tai dia chỉ 0: %d\n", val);
  printf("=== Ket thuc kiem thu ===\n");
  
  exit(0);
}