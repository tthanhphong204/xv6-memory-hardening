# xv6 Memory Hardening

Đồ án bổ sung cơ chế stack canary cho xv6 RISC-V. Hệ thống cấp một giá trị canary 64-bit riêng cho mỗi tiến trình, truyền canary sang user space khi chương trình bắt đầu và dùng GCC stack protector để kiểm tra giá trị đó trước khi protected function trả về.

## Tính năng

- Canary riêng cho từng tiến trình.
- Sinh canary bằng `random_u64()` trong quá trình cấp process.
- System call `getcanary` chuyển canary từ kernel sang user space.
- Khởi tạo `__stack_chk_guard` trong `start()`.
- GCC `-fstack-protector-strong` instrument các hàm user có nguy cơ overflow.
- Hàm `__stack_chk_fail()` kết thúc tiến trình với exit status `1`.
- Chương trình smoke test `canary`.
- Chương trình negative test `overflow`.

## Luồng hoạt động

1. `allocproc()` trong `kernel/proc.c` sinh `p->stack_canary`.
2. User process gọi system call `getcanary` lần đầu tại `start()` trong `user/ulib.c`.
3. `start()` gán giá trị nhận được cho `__stack_chk_guard`.
4. GCC lưu `__stack_chk_guard` trên stack khi vào protected function.
5. Khi function trả về, GCC XOR/so sánh giá trị stack-local với `__stack_chk_guard`.
6. Nếu khác nhau, GCC gọi `__stack_chk_fail()`; xv6 kết thúc tiến trình bằng `exit(1)`.

## Thành phần chính

- `kernel/proc.h`: trường `stack_canary` trong `struct proc`.
- `kernel/proc.c`: cấp và giải phóng canary cùng vòng đời process.
- `kernel/sysproc.c`: implementation syscall `getcanary`.
- `kernel/syscall.c`, `kernel/syscall.h`: đăng ký và số syscall.
- `kernel/random.c`: nguồn sinh giá trị 64-bit.
- `user/ulib.c`: khởi tạo guard và xử lý canary mismatch.
- `user/canary.c`: test trường hợp bình thường.
- `user/exploit_overflow.c`: test cố tình ghi vượt buffer stack.
- `Makefile`: bật `-fstack-protector-strong` cho user program.
- `docs/report.md`: báo cáo thiết kế, kiểm thử và đánh giá.

## Build

Cần có RISC-V GCC toolchain và `qemu-system-riscv64`.

```sh
make clean
make
make fs.img
make qemu
```

Trong shell xv6:

```text
$ canary
$ overflow
```

Kết quả mong đợi:

- `canary` in dòng `Stack canary normal test passed` và thoát thành công.
- `overflow` là negative test; canary mismatch phải ngăn function/tiến trình tiếp tục, không được in dòng `ERROR: overflow_test unexpectedly returned`.

## Kiểm tra assembly instrument

```sh
riscv64-unknown-elf-objdump -d user/_overflow
```

Trong assembly của `overflow_test`, cần thấy:

- load giá trị `__stack_chk_guard`;
- lưu guard vào stack-local;
- epilogue XOR/so sánh guard với bản giá trị trên stack;
- nhánh gọi `__stack_chk_fail`.
