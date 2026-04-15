# 🃏 Dự án Mô phỏng và Phân tích Dữ liệu Xì Dách

Dự án này sử dụng C++ để mô phỏng trò chơi Xì Dách, ghi lại dữ liệu ván đấu và phân tích xác suất thông qua MySQL và tạo biểu đồ bằng Python.

---

## 🛠 Tech Stack
* **Ngôn ngữ:** C++ (Standard 17+), Python
* **Cơ sở dữ liệu:** MySQL
* **Định dạng dữ liệu:** CSV
* **Công cụ:** Git, HeidiSQL / MySQL Workbench

## 🇻🇳 Luật chơi mô phỏng theo luật ở Việt Nam
* **Nhà Cái(Dealer):** Quyền lợi được xét bài sớm ngay khi đủ 15 điểm.
* **Thứ tự ưu tiên:** Nhà cái xét nhóm người chơi có 3-4 lá trước để tối ưu hoá lợi nhuận.
* **Bộ bài đặc biệt:** Xì Dách, Xì Bàng, Ngũ Linh.

## ✨ Tính năng chính
- [x] **Áp dụng OOP:** Thiết kế hướng đối tượng chặt chẽ với các class `Participant`, `Player`, `NhaCai`.
- [x] **Tự động hóa:** AI đưa ra quyết định (Hit/Stand) dựa trên điểm số hiện tại.
- [x] **Logging:** Ghi nhật ký chi tiết từng lượt rút bài và kết quả ván đấu ra file CSV.
- [x] **Data-Ready:** Dữ liệu được tối ưu hóa để import trực tiếp vào Database để phân tích.
- [x] **Vẽ biểu đồ:** Dùng thư viện Pandas và Mathplotlib trong Python để vẽ biểu đồ.

## 📊 Cấu trúc Database

### 1. Bảng `logturn` (Nhật ký từng lượt)
| Cột | Ý nghĩa |
| :--- | :--- |
| **GameID** | ID của ván đấu |
| **PlayerID** | ID của người chơi |
| **TurnNumber** | Lượt chơi hiện tại |
| **bCard** | Các lá bài trên tay trước khi ra quyết định |
| **bScore** | Điểm số trước khi ra quyết định |
| **Action** | Hành động thực hiện (Hit/Stand) |
| **dCard** | Lá bài vừa rút được (nếu chọn Hit) |
| **aScore** | Điểm số sau khi thực hiện hành động |

### 2. Bảng `loggameresult` (Kết quả ván đấu)
| Cột | Ý nghĩa |
| :--- | :--- |
| **GameID** | ID của ván đấu |
| **PlayerID** | ID của người chơi |
| **Result** | Kết quả của người chơi |
| **CurrentMoney** | Tiền hiện tại của người chơi |

### 3. Bảng `logcompareresuly` (Kết quả so sánh bài)
| Cột | Ý nghĩa |
| :--- | :--- |
| **GameID** | ID của ván đấu |
| **PlayerID** | ID của người chơi |
| **PlayerCard** | Bài của người chơi lúc xét |
| **PlayerScore** | Điểm của người chơi lúc xét |
| **DealerCard** | Bài của nhà cái lúc xét |
| **DealerScore** | Điểm của nhà cái lúc xét |
| **Result** | Kết quả của người chơi |

---

## 📈 Phân tích chiến thuật
Dự án so sánh 3 nhóm đối tượng điển hình:
1. **NC1 - Nhát Gan:** Luôn dằn bài khi đủ từ 16đ trở lên.
2. **NC2 - Tối Ưu:** Luôn dằn bài khi đủ từ 17đ trở lên.
3. **NC3 - Liều Lĩnh:** Luôn dằn bài khi đủ 18đ trở lên.

### Kết quả trực quan hoá:

**Nhận xét:** Sau 100.000 ván đấu, biểu đồ cho thấy Nhà Cái luôn có lợi thế dương về dài hạn. Người chơi "Tối Ưu" có độ dốc giảm tài sản thấp nhất, trong khi người chơi "Liều Lĩnh" có rủi ro cháy túi cao nhất.

---

## 📋 Cách cài đặt và sử dụng 
1. **Clone repo:** git clone https://github.com/your-username/blackjack-simulation.git
2. **Biên dịch:** Chạy file XiDach.cpp.
3. **Nhập số ván muốn mô phỏng.**
4. **Chạy file CreateChart.py để tạo biểu đồ theo dõi tiền.** (Yêu cầu phải cài thư viện Pandas và Mathplotlib)
5. **Load file CSV vào MySQL bằng các lệnh sau:**
	**Load file vào bảng logturn:**
	```sql
	LOAD DATA INFILE 'Đường dẫn tới file LogTurn.csv'
	INTO TABLE logturn
	FIELDS TERMINATED BY ','
	ENCLOSED BY '"'
	LINES TERMINATED BY '\r\n'
	IGNORE 1 ROWS;
	```
	**Load file vào bảng loggameresult:**
	```sql
	LOAD DATA INFILE 'Đường dẫn tới file LogGameResult.csv'
	INTO TABLE loggameresult
	FIELDS TERMINATED BY ','
	ENCLOSED BY '"'
	LINES TERMINATED BY '\r\n'
	IGNORE 1 ROWS;
	```
	**Load file vào bảng logcompareresult:**
	```sql
	LOAD DATA INFILE 'Đường dẫn tới file LogCompareResult.csv'
	INTO TABLE logcompareresult
	FIELDS TERMINATED BY ','
	ENCLOSED BY '"'
	LINES TERMINATED BY '\r\n'
	IGNORE 1 ROWS;
	```

---

## 📊 Một số lệnh SQL để phân tích:
1. **Thống kê tỷ lệ Thắng/Thua của từng chiến thuật:**
	```sql
	SELECT 
    	PlayerID,
    	COUNT(CASE WHEN Result = 'Win' THEN 1 END) AS TotalWins,
    	COUNT(CASE WHEN Result = 'Lose' THEN 1 END) AS TotalLosses,
    	COUNT(CASE WHEN Result = 'Tie' THEN 1 END) AS TotalTies,
    	ROUND(COUNT(CASE WHEN Result = 'Win' THEN 1 END) * 100.0 / COUNT(*), 2) AS WinRatePercentage
	FROM loggameresult
	GROUP BY PlayerID;
	```

2. **Phân tích lợi nhuận ròng cuối cùng**
	```sql
	SELECT 
    	PlayerID,
    	(MAX(CurrentMoney) - 1000) AS NetProfit,
    	MIN(CurrentMoney) AS LowestPoint,
    	MAX(CurrentMoney) AS HighestPoint
	FROM loggameresult
	GROUP BY PlayerID;
	```

3. **Kiểm tra Nhà Cái thắng bao nhiêu trận với mỗi người chơi:**
	```sql
	SELECT 
    	PlayerID, 
    	SUM(CASE WHEN Result = 'Lose' THEN 1 ELSE 0 END) AS GamesLostToDealer,
    	SUM(CASE WHEN Result = 'Lose' THEN 1 ELSE 0 END) * 1.0 / (SELECT COUNT(DISTINCT GameID) FROM loggameresult) AS DealerAdvantage
	FROM loggameresult
	WHERE PlayerID != 0
	GROUP BY PlayerID;
	```

4. **Kiểm tra xác suất Quắc khi rút ở các mốc điểm**
	```sql
	SELECT 
    	bScore AS ScoreBeforeHit,
    	COUNT(*) AS TotalHits,
    	SUM(CASE WHEN aScore > 21 THEN 1 ELSE 0 END) AS BustCount,
    	ROUND(SUM(CASE WHEN aScore > 21 THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS BustProbability
	FROM logturn
	WHERE ActionTaken = 'Hit'
	GROUP BY bScore
	ORDER BY bScore;
	```

5. **Kiểm tra số lượng ván thắng, thua, hoà của Nhà Cái khi xét với số lá tương ứng của Nhà Con:**
	```sql
	SELECT 
    	LENGTH(PlayerCard) - LENGTH(REPLACE(PlayerCard, ',', '')) AS CardCount,
    	Result,
    	COUNT(*) AS Occurrences
	FROM logcompareresult
	GROUP BY CardCount, Result
	ORDER BY CardCount;
	```
