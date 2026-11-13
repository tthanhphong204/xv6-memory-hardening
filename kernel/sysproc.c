#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  if(n > 0) {
    uint64 random_heap_gap = (rand() % 4) * 4096; // 4096 là kích thước 1 trang (PGSIZE)
    myproc()->sz += random_heap_gap;
  }
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
// ==========================================
// BẮT ĐẦU SYSTEM CALL MMAP CÓ TÍCH HỢP ASLR
// ==========================================
uint64 sys_mmap(void) {
  uint64 length;
  int prot, flags, fd, offset;

  // 1. Lấy các tham số mà tiến trình User gửi xuống
  argaddr(0, &length);
  argint(1, &prot);
  argint(2, &flags);
  argint(3, &fd);
  argint(4, &offset);

  struct proc *p = myproc();
  
  // 2. Tìm một khe VMA trống trong "sổ tay" của tiến trình
  struct vma *v = 0;
  for(int i = 0; i < NVMA; i++) {
    if(p->vmas[i].valid == 0) {
      v = &p->vmas[i];
      break;
    }
  }
  if(v == 0) return -1; // Báo lỗi nếu tiến trình đã dùng hết 16 vùng VMA

  // 3. THUẬT TOÁN ASLR (Ngẫu nhiên hóa vị trí)
  // Làm tròn kích thước yêu cầu lên bội số của trang nhớ (4096 bytes)
  length = PGROUNDUP(length);
  
  // Cấp phát theo hướng từ trên xuống dưới (gắn với aslr_mmap_base đã sinh ngẫu nhiên ở proc.c)
  p->aslr_mmap_base -= length;
  uint64 alloc_addr = p->aslr_mmap_base;

  // 4. Ghi chép thông tin vào cấu trúc VMA để quản lý
  v->valid = 1;
  v->addr = alloc_addr;
  v->length = length;
  v->prot = prot;
  v->flags = flags;
  // (Lưu ý: Phần map với file vật lý trên ổ cứng sẽ được làm chi tiết vào tuần sau)
  
  // Trả về địa chỉ ngẫu nhiên cho User-space
  return alloc_addr;
}
// ==========================================
// KẾT THÚC SYSTEM CALL MMAP
// ==========================================