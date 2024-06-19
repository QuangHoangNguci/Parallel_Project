import pandas as pd
import matplotlib.pyplot as plt

# Đọc dữ liệu từ file CSV
df = pd.read_csv('result.csv', header=None)

# Đặt tên cho các cột (giả sử cột đầu tiên là kích thước đầu vào, cột thứ hai là số lượng luồng CPU, tiếp theo là thời gian với và không có truyền thông)
df.columns = ['Input_size', 'Num_threads', 'Time_with_comm', 'Time_without_comm']

# Vẽ biểu đồ thời gian với và không có truyền thông theo kích thước đầu vào
plt.figure(figsize=(14, 7))

for num_threads in df['Num_threads'].unique():
    subset = df[df['Num_threads'] == num_threads]
    plt.plot(subset['Input_size'], subset['Time_with_comm'], label=f'{num_threads} luồng - Thời gian có truyền thông', marker='o')
    plt.plot(subset['Input_size'], subset['Time_without_comm'], label=f'{num_threads} luồng - Thời gian không truyền thông', marker='x')

plt.xlabel('Kích thước đầu vào (số phần tử)')
plt.ylabel('Thời gian (giây)')
plt.title('Thời gian thực hiện sắp xếp với và không có truyền thông theo kích thước đầu vào')
plt.legend()
plt.grid(True)
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()

# Vẽ biểu đồ thời gian với và không có truyền thông theo số lượng luồng CPU
plt.figure(figsize=(14, 7))

for input_size in df['Input_size'].unique():
    subset = df[df['Input_size'] == input_size]
    plt.plot(subset['Num_threads'], subset['Time_with_comm'], label=f'Kích thước {input_size} - Thời gian có truyền thông', marker='o')
    plt.plot(subset['Num_threads'], subset['Time_without_comm'], label=f'Kích thước {input_size} - Thời gian không truyền thông', marker='x')

plt.xlabel('Số lượng luồng CPU')
plt.ylabel('Thời gian (giây)')
plt.title('Thời gian thực hiện sắp xếp với và không có truyền thông theo số lượng luồng CPU')
plt.legend()
plt.grid(True)
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()
