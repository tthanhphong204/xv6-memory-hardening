#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int local_var = 42; // Biến cục bộ nằm trên Stack
  
  printf("ASLR Test: Dia chi stack cua bien local_var la: %p\n", (void*)&local_var);
  
  exit(0);
}