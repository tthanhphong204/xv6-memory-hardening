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
}