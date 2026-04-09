# 🃏 Dự án Mô phỏng và Phân tích Dữ liệu Xì Dách

Dự án này sử dụng C++ để mô phỏng trò chơi Xì Dách, ghi lại dữ liệu ván đấu và phân tích xác suất thông qua MySQL.

---

## 🛠 Tech Stack
* **Ngôn ngữ:** C++ (Standard 17+)
* **Cơ sở dữ liệu:** MySQL
* **Định dạng dữ liệu:** CSV
* **Công cụ:** Git, HeidiSQL / MySQL Workbench

## ✨ Tính năng chính
- [x] **Áp dụng OOP:** Thiết kế hướng đối tượng chặt chẽ với các class `Participant`, `Player`, `NhaCai`.
- [x] **Tự động hóa:** AI đưa ra quyết định (Hit/Stand) dựa trên điểm số hiện tại.
- [x] **Logging:** Ghi nhật ký chi tiết từng lượt rút bài và kết quả ván đấu ra file CSV.
- [x] **Data-Ready:** Dữ liệu được tối ưu hóa để import trực tiếp vào Database để phân tích.

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

---

## 📈 Phân tích dữ liệu (SQL Insights)
Dự án tập trung vào việc trả lời các câu hỏi thực tế thông qua SQL:
1. **Chiến thuật:** Tại 17 điểm, tỉ lệ thua khi "Rút" so với "Dừng" là bao nhiêu?
2. **Vị trí:** Thứ tự ngồi (Player 1-6) có ảnh hưởng đến xác suất thắng không?
3. **Chủ bàn:** Số lượng người chơi bao nhiêu thì Dealer có lợi thế lớn nhất?

---

## 🚀 Cách chạy chương trình
1. Compile file XiDach.cpp bằng g++.
2. Nhập số lượng game muốn giả lập.
3. Mở MySQL, chạy các script sau:
   * Load dữ liệu vào bảng logturn:
     ```sql
		 LOAD DATA INFILE 'Đường dẫn tới file LogTurn.csv'
		 INTO TABLE logturn
		 FIELDS TERMINATED BY ','
		 ENCLOSED BY '"'
		 LINES TERMINATED BY '\r\n'
		 IGNORE 1 ROWS;
     ```
   * Load dữ liệu vào bảng loggameresult:
     ```sql
     LOAD DATA INFILE 'Đường dẫn tới file LogGameResult.csv'
		 INTO TABLE loggameresult
		 FIELDS TERMINATED BY ','
		 ENCLOSED BY '"'
		 LINES TERMINATED BY '\r\n'
		 IGNORE 1 ROWS;
     ```
     
---

## 📊 Một số câu lệnh SQL để phân tích:
1. Tại 16 điểm, tỉ lệ thua khi "Rút" so với "Dừng" là bao nhiêu?
   ```sql
   SELECT ACTION,
		 COUNT(*) AS Total_Cases,
		 SUM(CASE WHEN r.Result = 'Lose' THEN 1 ELSE 0 END) AS Lose_Cases,
		 SUM(CASE WHEN r.Result = 'Lose' THEN 1 ELSE 0 END) / COUNT(*) AS Lose_Rates
	 FROM logturn t
	 JOIN loggameresult r ON t.GameID = r.GameID AND t.PlayerID = r.PlayerID
	 WHERE t.bScore = 16 AND t.PlayerID != 0 AND t.`Action` != 'Ngu Linh'
	 GROUP BY ACTION;
   ```
2. Tỉ lệ nhà cái thắng nếu dừng từ 18đ trở xuống
