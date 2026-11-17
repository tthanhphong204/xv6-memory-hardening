#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  printf("Exploit Null Dereference Test START\n");

  // Cố tình trỏ con trỏ về địa chỉ 0 (NULL) và đọc giá trị
  int *null_ptr = (int *)0;
  int val = *null_ptr;

  printf("Value at NULL: %d\n", val);
  printf("Exploit Null Dereference Test FAILED (Should have been killed!)\n");

  exit(0);
}