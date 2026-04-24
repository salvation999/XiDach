import pandas as pd
import matplotlib.pyplot as plt

# 1. Đọc dữ liệu từ file CSV
df = pd.read_csv('LogGameResult.csv')

# 2. Thiết lập kích thước biểu đồ
plt.figure(figsize=(14, 7))

# 3. Tự động lấy danh sách PlayerID
all_player_id = sorted(df['PlayerID'].unique())
# Xác định chế độ để vẽ biểu đồ
with open('config.txt', 'r') as f:
    current_mode = int(f.read().strip())

# 4. Vẽ biểu đồ
for pid in all_player_id:
    player_data = df[df['PlayerID'] == pid]
    if pid == 0:
        label_name = "Nhà Cái"
    elif (current_mode == 1):
        labels = {1 : 'NC1 - Nhát Gan', 2 : 'NC2 - Tối Ưu', 3 : 'NC3 - Liều Lĩnh'}
        label_name = labels.get(pid, f'Player {pid}')
    else:
        label_name = f'Player {pid}'
    plt.plot(player_data['GameID'], player_data['CurrentMoney'], label = label_name)
# 4. Trang trí biểu đồ
plt.title('Phân tích Dòng tiền Xì Dách sau mô phỏng 100,000 ván', fontsize=14)
plt.xlabel('Số lượng ván đấu (GameID)', fontsize=12)
plt.ylabel('Số tiền hiện có (CurrentMoney)', fontsize=12)
plt.axhline(y=5000, color='gray', linestyle='--', alpha=0.5) # Đường vốn ban đầu
plt.tight_layout()
plt.legend()
plt.grid(True, linestyle=':', alpha=0.6)

# 5. Lưu biểu đồ ra file ảnh
plt.savefig('Blackjack_Financial_Analysis.png')
plt.show()