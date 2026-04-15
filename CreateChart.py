import pandas as pd
import matplotlib.pyplot as plt

# 1. Đọc dữ liệu từ file CSV
df = pd.read_csv('LogGameResult.csv')

# 2. Thiết lập kích thước biểu đồ
plt.figure(figsize=(12, 6))

# 3. Vẽ đường biểu diễn cho từng người chơi
# PlayerID 0: Nhà cái, 1: Nhát gan, 2: Tối ưu, 3: Máu liều
players = {
    0: 'Nhà Cái (Dealer)',
    1: 'NC1 - Nhát Gan (Dằn 15)',
    2: 'NC2 - Tối Ưu (Dằn 17)',
    3: 'NC3 - Máu Liều (Dằn 18)'
}

for pid, name in players.items():
    player_data = df[df['PlayerID'] == pid]
    plt.plot(player_data['GameID'], player_data['CurrentMoney'], label=name)

# 4. Trang trí biểu đồ
plt.title('Phân tích Dòng tiền Xì Dách sau mô phỏng 10,000 ván', fontsize=14)
plt.xlabel('Số lượng ván đấu (GameID)', fontsize=12)
plt.ylabel('Số tiền hiện có (CurrentMoney)', fontsize=12)
plt.axhline(y=100000, color='gray', linestyle='--', alpha=0.5) # Đường vốn ban đầu
plt.legend()
plt.grid(True, linestyle=':', alpha=0.6)

# 5. Lưu biểu đồ ra file ảnh
plt.savefig('Blackjack_Financial_Analysis.png')
plt.show()