import pexpect
import re
import csv

def run_experiment(iterations=1000, output_file='scripts/entropy_data.csv'):
    print(f"🚀 Bắt đầu khởi động xv6 qua QEMU để thu thập {iterations} mẫu...")
    
    # Khởi động xv6
    child = pexpect.spawn('make qemu', encoding='utf-8', timeout=10)
    child.expect(r'\$ ')
    print("✅ Hệ thống đã sẵn sàng. Đang tiến hành chạy lệnh tự động...")
    
    with open(output_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Run', 'Stack_Address', 'Heap_Address'])
        
        for i in range(1, iterations + 1):
            child.sendline('entropy_dump')
            child.expect(r'\$ ') 
            output = child.before
            
            stack_match = re.search(r'Stack address:\s*(0x[0-9a-fA-F]+)', output)
            heap_match = re.search(r'Heap address:\s*(0x[0-9a-fA-F]+)', output)
            
            if stack_match and heap_match:
                writer.writerow([i, stack_match.group(1), heap_match.group(1)])
                if i % 100 == 0:
                    print(f"⏳ Đã thu thập {i}/{iterations} mẫu...")
            else:
                print(f"⚠️ Lỗi đọc dữ liệu lần {i}.")
                
    # Lệnh thoát QEMU tự động (tương đương Ctrl+A, X)
    child.send('\x01x')
    print(f"🎉 Hoàn tất! File dữ liệu nằm tại: {output_file}")

if __name__ == '__main__':
    run_experiment(1000, 'scripts/entropy_data.csv')