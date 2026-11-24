# Báo cáo đồ án: Stack Canary cho xv6

## 1. Mục tiêu

Bổ sung stack canary cho user process trong xv6 RISC-V, bảo vệ stack frame khỏi buffer overflow cơ bản bằng cơ chế được compiler tự động instrument.

Kết quả cần đạt:

- Mỗi tiến trình có một giá trị canary riêng.
- Canary được khởi tạo trước khi user program chạy.
- Protected function lưu và kiểm tra canary thực sự trước khi trả về.
- Negative test ghi vượt vùng buffer phải kích hoạt cơ chế phát hiện.
- Kernel và userland build thành công.
- Có tài liệu kiến trúc, build và kiểm thử.

## 2. Thiết kế

### 2.1 Canary theo tiến trình

`struct proc` trong `kernel/proc.h` lưu canary 64-bit bằng:

```c
uint64 stack_canary;
```

Trong `allocproc()` (`kernel/proc.c`), canary được sinh bằng `random_u64()` và gán cho process mới:

```c
p->stack_canary = random_u64();
```

Giá trị có được đặt lại `0` trong `freeproc()` để tránh để dữ liệu canary sống sót sau khi process slot được tái sử dụng.

Lưu ý: implementation `random_u64()` hiện là pseudo-random dựa trên timer, địa chỉ và xorshift. Với mục tiêu lab, giá trị này đã khác nhau theo từng process; nó chưa phải entropy cryptographic.

### 2.2 Truyền canary sang user space

System call `getcanary` được:

- khai báo trong `kernel/syscall.h`;
- đăng ký trong syscall dispatch table;
- implement trong `kernel/sysproc.c`;
- khai báo cho user chương trình trong `user/user.h`.

Khi tiến trình bắt đầu bằng `start()` trong `user/ulib.c`, kernel canary được lấy một lần:

```c
__stack_chk_guard = getcanary();
```

Giá trị này trở thành `__stack_chk_guard` chung của user process.

### 2.3 Instrument của compiler

`Makefile` dùng `-fstack-protector-strong` cho mọi user program:

```make
USER_CFLAGS = $(filter-out -fno-stack-protector,$(CFLAGS)) \
              -fstack-protector-strong
```

Với hàm có local array hoặc cấu trúc nguy cơ, GCC:

1. load `__stack_chk_guard`;
2. lưu guard vào stack frame;
3. thực hiện nội dung hàm;
4. so sánh guard trên stack với `__stack_chk_guard`;
5. gọi `__stack_chk_fail()` nếu guard đã thay đổi.

Kết quả assembly cho `overflow_test` có thể kiểm tra bằng:

```sh
riscv64-unknown-elf-objdump -d user/_overflow
```

Assembly đã chứng minh epilogue chứa load guard, XOR/so sánh và nhánh tới `__stack_chk_fail`.

### 2.4 Xử lý khi mismatch

Trong `user/ulib.c`:

```c
__attribute__((no_stack_protector))
void __stack_chk_fail(void)
{
  write(1, "STACK CANARY MISMATCH\n", 22);
  exit(1);
}
```

Như vậy, một mismatch kết thúc tiến trình thay vì cho phép hàm trả về với return address/stack frame đã bị chỉnh sửa.

## 3. Chương trình kiểm thử

### 3.1 Test bình thường

`user/canary.c` chạy hàm `normal_test()` với local array cố định kích thước hợp lệ. Kết quả mong đợi:

```text
Normal test: buffer[0] = A
Normal function completed.
Stack canary normal test passed.
```

Test này xác nhận guard được khởi tạo và không phát cảnh báo sai trong stack hợp lệ.

### 3.2 Negative test buffer overflow

`user/exploit_overflow.c` khai báo buffer 16 byte và cố ghi ngoài vùng bằng pointer truy cập 80 byte:

```c
volatile char buffer[16];
volatile uint64 *guard_slot =
    (volatile uint64 *)(void *)&buffer[16];

smash_stack(buffer, 80);
*guard_slot = 0;
```

Sau vòng ghi, test ghi trực tiếp giá trị `0` vào guard slot để tạo mismatch xác định. Nếu canary hoạt động, compiler epilogue phát hiện mismatch và gọi `__stack_chk_fail()`.

Negative test được đặt tên user binary ngắn là `overflow` để không vượt giới hạn tên file trong xv6 filesystem:

```make
$U/_overflow
```

## 4. Kết quả kiểm thử

### 4.1 Build kernel

Kết quả build kernel thành công:

- `make -j4` tạo `kernel/kernel`.
- Không còn duplicate symbol `sys_getcanary` giữa `sysproc.c` và `syscall.c`.
- QEMU boot được với kernel và init shell.

### 4.2 Build userland và filesystem

`make fs.img` tạo thành công:

- `user/_canary`;
- `user/_overflow`;
- `fs.img`.

Đã sửa tên binary từ `_exploit_overflow` sang `_overflow` vì `DIRSIZ` giới hạn tên file xv6.

### 4.3 Runtime smoke test

Trong shell QEMU:

```text
$ canary
Normal test: buffer[0] = A
Normal function completed.
Stack canary normal test passed
```

Kết quả xác nhận canary không bị mismatch trong luồng bình thường.

### 4.4 Runtime overflow test

Trong shell QEMU:

```text
$ overflow
Attempting stack-smash overflow as a negative test.
Overflow test starting...
STACK CANARY MISMATCH
```

Test ghi 80 byte qua buffer 16 byte và ghi trực tiếp giá trị `0` vào guard slot tương ứng. Output runtime xác nhận compiler epilogue phát hiện mismatch và `__stack_chk_fail()` thực thi. Tiến trình test thoát với status `1`.

### 4.5 Kiểm tra compiler instrumentation

Assembly của `overflow_test` xác nhận:

- local guard được lưu tại offset âm của frame;
- guard được load lại tại epilogue;
- guard được XOR/so sánh với giá trị global;
- `__stack_chk_fail` là nhánh sai.

## 5. Hạn chế

- Stack protector chỉ được bật cho user program, không phải kernel.
- Phạm vi hiện tại là phát hiện stack corruption tại protected function epilogue, không ngăn mọi illegal memory access.
- Canary chưa được random cryptographically.
- GCC instrument phụ thuộc compiler flag và không bảo vệ mọi trường hợp bằng ngôn ngữ C.
- Không bổ sung ASLR, NX và control-flow integrity.

## 6. Kết luận

Đồ án đã tích hợp stack canary theo process trong xv6, truyền giá trị qua system call, khởi tạo guard trong runtime user và kiểm tra guard bởi GCC trước khi protected function trả về.

Các thành phẩm chính:

- `kernel/proc.h`, `kernel/proc.c`: thiết kế canary theo process.
- `kernel/sysproc.c`, `kernel/syscall.c`, `kernel/syscall.h`: system call canary.
- `user/ulib.c`: guard initialization và fail handler.
- `Makefile`: compiler instrumentation.
- `user/canary.c`: test bình thường.
- `user/exploit_overflow.c`: negative overflow test.
- `README.md` và `docs/report.md`: tài liệu và báo cáo.