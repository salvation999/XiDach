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
   ```sql
   WITH DealerFinalScore AS (
    SELECT t.gameID, t.aScore AS FinalScore
    FROM logturn t
    INNER JOIN (
        SELECT gameID, MAX(turnNumber) as max_turn
        FROM logturn
        WHERE PlayerID = 0
        GROUP BY gameID
    ) m ON t.gameID = m.gameID AND t.turnNumber = m.matx_turn
    WHERE t.PlayerID = 0
   )
   SELECT 
    f.FinalScore,
    COUNT(*) AS Total_Games,
    SUM(CASE WHEN r.Result = 'Win' THEN 1 ELSE 0 END) AS Wins,
    SUM(CASE WHEN r.Result = 'Lose' THEN 1 ELSE 0 END) AS Losses,
    ROUND(SUM(CASE WHEN r.Result = 'Win' THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS Win_Rate_Percentage
   FROM loggameresult r
   JOIN DealerFinalScore f ON r.gameID = f.gameID
   WHERE r.PlayerID = 0 AND f.FinalScore < 19 AND f.FinalScore > 15
   GROUP BY f.FinalScore
   ORDER BY f.FinalScore DESC;
   ```
3. Kiểm tra người chơi dừng ở bao nhiêu điểm thì có tỉ lệ thắng cao
   ```sql
   SELECT 
    f.aScore AS FinalScore,
    COUNT(*) AS Total_Games,
    SUM(CASE WHEN r.Result = 'Win' THEN 1 ELSE 0 END) AS Wins,
    ROUND(SUM(CASE WHEN r.Result = 'Win' THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS Win_Rate
   FROM loggameresult r
   JOIN (
    SELECT gameID, PlayerID, aScore 
    FROM logturn 
    WHERE Action = 'Stand'
   ) f ON r.gameID = f.gameID AND r.PlayerID = f.PlayerID
   WHERE r.PlayerID != 0
   GROUP BY FinalScore
   ORDER BY FinalScore DESC;
   ```
4. Kiểm tra xác suất Nhà Cái thắng ít nhất 1 người và xác suất thắng được cả bàn
   ```sql
   SELECT 
    COUNT(*) AS Total_Games_Simulated,
    SUM(CASE WHEN Players_Lost >= 1 THEN 1 ELSE 0 END) AS Games_Dealer_Won_At_Least_One,
    ROUND(SUM(CASE WHEN Players_Lost >= 1 THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS Prob_Win_At_Least_One_Percent,
    SUM(CASE WHEN Players_Lost = Total_Players_In_Game THEN 1 ELSE 0 END) AS Games_Dealer_Swept,
    ROUND(SUM(CASE WHEN Players_Lost = Total_Players_In_Game THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS Prob_Sweep_Percent
   FROM (
    SELECT 
        gameID, 
        COUNT(*) AS Total_Players_In_Game,
        SUM(CASE WHEN Result = 'Lose' THEN 1 ELSE 0 END) AS Players_Lost
    FROM loggameresult
    WHERE PlayerID != 0
    GROUP BY gameID
   ) AS GameSummary;
   ```
5. Kiểm tra tỷ lệ thắng của Nhà Cái với số lượng người chơi khác nhau
   ```sql
   SELECT 
    sub.NumPlayers AS Players_In_Table,
    COUNT(*) AS Total_Games,
    SUM(CASE WHEN r.Result = 'Win' THEN 1 ELSE 0 END) AS Dealer_Wins,
    SUM(CASE WHEN r.Result = 'Lose' THEN 1 ELSE 0 END) AS Dealer_Losses,
    SUM(CASE WHEN r.Result = 'Tie' THEN 1 ELSE 0 END) AS Dealer_Ties,
    ROUND(SUM(CASE WHEN r.Result = 'Win' THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) AS Dealer_Win_Rate_Percent
   FROM loggameresult r
   JOIN (
    SELECT gameID, COUNT(DISTINCT PlayerID) as NumPlayers 
    FROM loggameresult 
    WHERE PlayerID != 0 
    GROUP BY gameID
   ) sub ON r.gameID = sub.gameID
   WHERE r.PlayerID = 0
   GROUP BY sub.NumPlayers
   ORDER BY sub.NumPlayers ASC;
