import csv
import math
from collections import Counter

def calculate_entropy(data):
    counts = Counter(data)
    total = len(data)
    entropy = -sum((count/total) * math.log2(count/total) for count in counts.values())
    return entropy, len(counts)

stack_addrs = []
heap_addrs = []

with open('scripts/entropy_data.csv', 'r') as file:
    reader = csv.DictReader(file)
    for row in reader:
        stack_addrs.append(row['Stack_Address'])
        heap_addrs.append(row['Heap_Address'])

stack_ent, stack_unique = calculate_entropy(stack_addrs)
heap_ent, heap_unique = calculate_entropy(heap_addrs)

print(f"--- KẾT QUẢ PHÂN TÍCH ASLR (1000 MẪU) ---")
print(f"Vùng nhớ Stack: {stack_unique} địa chỉ khác nhau | Độ phân tán (Entropy): {stack_ent:.2f} bits")
print(f"Vùng nhớ Heap:  {heap_unique} địa chỉ khác nhau | Độ phân tán (Entropy): {heap_ent:.2f} bits")