import pandas as pd
import matplotlib.pyplot as plt

# Đọc file CSV
data = pd.read_csv('results.csv', names=['Dimension', 'T_w_com', 'T_wo_com'])

# Vẽ biểu đồ
plt.figure(figsize=(10, 6))
plt.plot(data['Dimension'], data['T_w_com'], label='Time with communication')
plt.plot(data['Dimension'], data['T_wo_com'], label='Time without communication')

plt.xlabel('n: input data size')
plt.ylabel('Time (s)')
plt.title('Performance of Dijkstra Algorithm')
plt.legend()
plt.grid(True)
plt.show()

