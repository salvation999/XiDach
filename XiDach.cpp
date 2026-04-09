#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

void LogTurn(int gameID, int PlayerID, int turnNumber, string Card, int bScore, string Action, string DrawCard, int aScore) {
    string fileName = "LogTurn.csv";
    bool isNewFile = false;
    ifstream checkFile(fileName);
    if (!checkFile.is_open()) {
        isNewFile = true; // Nếu file không tồn tại, sẽ tạo mới
    }
    checkFile.close();
    ofstream outFile("LogTurn.csv", ios::app);
    if (outFile.is_open()) {
        if (isNewFile) {
            outFile << "GameID,PlayerID,TurnNumber,CardBeforeAction,ScoreBeforeAction,ActionTaken,CardDrawn,ScoreAfterAction\n";
        }
        outFile << gameID << "," 
        << PlayerID << "," 
        << turnNumber << ",\"" 
        << Card << "\"," 
        << bScore << "," 
        << Action << ",\"" 
        << DrawCard << "\"," 
        << aScore << "\n";
        outFile.close();
    }
}

void LogGameResult(int gameID, int PlayerID, string Result) {
    string fileName = "LogGameResult.csv";
    bool isNewFile = false;
    ifstream checkFile(fileName);
    if (!checkFile.is_open()) {
        isNewFile = true; // Nếu file không tồn tại, sẽ tạo mới
    }
    checkFile.close();

    ofstream outFile("LogGameResult.csv", ios::app);
    if (outFile.is_open()) {
        if (isNewFile) {
            outFile << "GameID,PlayerID,Result\n";
        } 
        outFile << gameID << "," 
        << PlayerID << ",\"" 
        << Result << "\"\n";
        outFile.close();
    }
}

class Card {
private:
    string suit;
    string rank;
public:
    Card(string s, string r) : suit(s), rank(r) {}
    string getSuit() const { return suit; }
    string getRank() const { return rank; }
};

class Deck {
private:
    vector<Card> cards; 
public:
    Deck() {
        string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
        string ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
        for (const auto& suit : suits) {
            for (const auto& rank : ranks) {
                cards.emplace_back(suit, rank);
            }
        }
    }

    void shuffle() {
        random_device rd; // Lấy một số ngẫu nhiên từ thiết bị
        mt19937 g(rd()); // Khởi tạo bộ sinh số ngẫu nhiên với seed từ thiết bị
        std::shuffle(cards.begin(), cards.end(), g); // Trộn bộ bài sử dụng thuật toán Fisher-Yates và bộ sinh số ngẫu nhiên
    }

    Card drawCard() {
        if (cards.empty()) {
            throw runtime_error("No more cards in the deck");
        }
        Card drawnCard = cards.back();
        cards.pop_back();
        return drawnCard;
    }
};

class Participant {
private:
    int ID;
    vector<Card> hand;
    int score;
    int numberOfAces; // Kiểm tra số lượng Aces trong tay để xử lý điểm số chính xác
    int numberOfAcesIs1; // Kiểm tra số lượng Aces đã được tính là 1 để điều chỉnh điểm số khi cần thiết
protected:
    string Action; // Lưu hành động của người chơi (Hit hoặc Stand)
public:
    Participant(int id) : ID(id), score(0), Action(""), numberOfAces(0), numberOfAcesIs1(0) {}

    void SetScore(int s) { score = s; }

    int getID() const { return ID; }

    void updateScore(Card card) {
        string rank = card.getRank();
        if (rank == "J" || rank == "Q" || rank == "K") {
            score += 10;
        } else if (rank == "A") {
            numberOfAces++; // Tăng số lượng Aces trong tay
            if (score + 11 <= 21 && (getHandSize() <= 3)) {
                score += 11; // A có thể được tính là 11 nếu cộng lại không vượt quá 21 và số lượng bài trong tay không quá 3
            } 
            else if (score + 10 <= 21 && getHandSize() == 3) {
                score += 10; // A có thể được tính là 10 nếu cộng lại không vượt quá 21 và số lượng bài trong tay là 3
            }
            else {
                score += 1; // A được tính là 1
                numberOfAcesIs1++; // Tăng số lượng Aces đã được tính là 1
            }
        } else {
            score += stoi(rank); // Đổi rank từ string sang int và cộng vào điểm số
        }
        if (getHandSize() == 4) {
            while (numberOfAcesIs1 < numberOfAces) {
                score -= 10; // Đổi A được tính là 11 hoặc 10 thành 1 nếu số lượng bài trong tay là 4
                numberOfAcesIs1++; // Tăng số lượng Aces đã được tính là 1
            }
        }
        if (score > 21 && numberOfAcesIs1 < numberOfAces) {
            if (score == 22 && getHandSize() == 3) {
                score -= 1; // Đổi A được tính là 11 thành 10 nếu điểm bằng 22 và số lượng bài trong tay là 3
            }
            else {
                score -= 10; // Đổi A được tính là 11 thành 1 nếu điểm vượt quá 22 
                numberOfAcesIs1++; // Tăng số lượng Aces đã được tính là 1
            }
        }
    }

    void receiveCard(Card card) {
        hand.push_back(card);
        // Cập nhật điểm số sau khi nhận được một lá bài mới
        updateScore(card);
    }

    vector<Card> getHand() const { return hand; }

    int getHandSize() const { return hand.size(); }

    int getScore() const { return score; }

    int SpecialWin() const {
        if (getScore() == 21 && getHandSize() == 2) {
            return 2;
        }
        else if (getHand()[0].getRank() == "A" && getHand()[1].getRank() == "A") {
                return 3;
        }   
        else if (getScore() <= 21 && getHandSize() == 5) {
            return 1;
        }   
        return 0;
    }

    string getHandString() {
        string handStr;
        for (const auto& card : hand) {
            handStr += card.getRank() + card.getSuit()[0] + ", ";
        }
        return handStr;
    }

    virtual void MakeDecision(Deck& deck) = 0;  // Hàm ảo thuần túy để các lớp con triển khai logic quyết định riêng

    void PlayerTurn(Deck& deck, int gameID) {
        int turnNumber = 1; // Khởi tạo lượt chơi đầu tiên
        string sCard  = getHandString();
        if (SpecialWin() > 0) {
            Action = "Stand"; // Nếu có hand đặc biệt, tự động dừng lại
            // Ghi log cho trường hợp thắng đặc biệt
            if (SpecialWin() == 2) {
                LogTurn(gameID, ID, turnNumber, sCard, getScore(), "Xi Dach", "", getScore());
            }
            else if (SpecialWin() == 3) {
                LogTurn(gameID, ID, turnNumber, sCard, getScore(), "Xi Ban", "", getScore());
            }
            return;
        }
        while (Action != "Stand") {
            int bScore = getScore();
            MakeDecision(deck);
            if (Action == "Hit") {
                string dCard = getHand().back().getRank() + getHand().back().getSuit()[0];
                LogTurn(gameID, ID, turnNumber, sCard, bScore, Action, dCard, getScore());
                sCard = getHandString(); // Cập nhật chuỗi bài sau khi nhận được lá bài mới
            } 
            else if (getHandSize() == 5 && getScore() <= 21) {
                LogTurn(gameID, ID, turnNumber, sCard, bScore, "Ngu Linh", "", getScore());
            }
            else if (getHandSize() == 5 && getScore() > 21) {
                LogTurn(gameID, ID, turnNumber, sCard, bScore, Action, "", getScore());
            }
            else {
                LogTurn(gameID, ID, turnNumber, sCard, bScore, Action, "", getScore());
            }
            turnNumber++;
        }
    }

};

class Player : public Participant {
public:
    Player(int id) : Participant(id) {}
    int getPlayerID() const { return getID(); }
    string getAction() const { return Action; }
    void MakeDecision(Deck& deck) override {
        // Logic quyết định của người chơi dựa trên điểm số hiện tại và số lượng bài trong tay
        if (getHandSize() == 5) {
            Action = "Stand"; // Người chơi quyết định dừng lại nếu đã có 5 lá bài trong tay
            return;
        }
        if (getScore() < 16) {
            receiveCard(deck.drawCard()); // Người chơi quyết định rút thêm bài nếu điểm số dưới 16
            Action = "Hit";
        } else if (getScore() == 16 || getScore() == 17) {
            int decision = rand() % 2; // Ngẫu nhiên quyết định rút thêm bài hoặc dừng lại nếu điểm số là 16 hoặc 17
            if (decision == 0) {
                receiveCard(deck.drawCard()); // Người chơi quyết định rút thêm bài
                Action = "Hit";
            } else {
                Action = "Stand"; // Người chơi quyết định dừng lại
            }
        } else {
            Action = "Stand"; // Người chơi quyết định dừng lại nếu điểm số là 18 hoặc cao hơn
        }
    }
};

class Dealer : public Participant {
public:
    Dealer(int id) : Participant(id) {}
    int getDealerID() const { return getID(); }
    string getAction() const { return Action; }
    void MakeDecision(Deck& deck) override {
        // Logic quyết định của dealer dựa trên điểm số hiện tại và số lượng bài trong tay
        if (getHandSize() == 5) {
            Action = "Stand"; // Dealer quyết định dừng lại nếu đã có 5 lá bài trong tay
            return;
        }
        // Rút bài nếu điểm số dưới 16
        if (getScore() < 16) {
            receiveCard(deck.drawCard());
            Action = "Hit";
        }
        else if (getScore() == 16 || getScore() == 17 || getScore() == 18) {
            int decision = rand() % 2; // Ngẫu nhiên quyết định rút thêm bài hoặc dừng lại nếu điểm số là 16, 17 hoặc 18
            if (decision == 0) {
                receiveCard(deck.drawCard()); // Dealer quyết định rút thêm bài
                Action = "Hit";
            } else {
                Action = "Stand"; // Dealer quyết định dừng lại
            }
        }
        else {
            Action = "Stand"; // Dealer quyết định dừng lại nếu điểm số là 19 hoặc cao hơn
        }
    }
};

class Game {
private:
    Deck deck;
    vector<Player> players;
    Dealer dealer;
    int numberOfPlayersWin, numberOfPlayersLose, numberOfPlayersTie;
public:
    Game(int numPlayers) : dealer(0), numberOfPlayersWin(0), numberOfPlayersLose(0), numberOfPlayersTie(0) {
        for (int i = 1; i <= numPlayers; ++i) {
            players.emplace_back(Player(i));
        }
    }

    void InitialDeal() {
        for (auto& player : players) {
            player.receiveCard(deck.drawCard());
        }
        dealer.receiveCard(deck.drawCard());
    }

    string CheckResult(const Dealer& dealer,const Player& player) {
        int dealerScore = dealer.getScore();
        int playerScore = player.getScore();
        if (player.SpecialWin() > 0) {
            if (dealer.SpecialWin() > player.SpecialWin()) {
                return "Lose";
            } else if (dealer.SpecialWin() < player.SpecialWin()) {
                return "Win";
            } else if (dealer.SpecialWin() == 1 && player.SpecialWin() == 1) {
                if (dealerScore > playerScore) {
                    return "Win";
                } else if (dealerScore < playerScore) {
                    return "Lose";
                } else {
                    return "Tie";
                }
            } else {
                return "Tie";
            }
        }
        else if (playerScore > 21) {
            if (dealerScore > 21) {
                return "Tie";
            } else {
                return "Lose";
            }
        } else if (dealerScore > 21 && playerScore <= 21) {
            return "Win";
        } else if (playerScore > dealerScore) {
            return "Win";
        } else if (playerScore < dealerScore) {
            return "Lose";
        } 
        else {
            return "Tie";
        }
    }

    void Result(int gameID) {
        string Result ="";
        for (const Player& player : players) {
            Result = CheckResult(dealer, player);
            if (Result == "Win") {
                numberOfPlayersWin++;
            }
            else if (Result == "Lose") {
                numberOfPlayersLose++;
            }
            else {
                numberOfPlayersTie++;
            }
            LogGameResult(gameID, player.getPlayerID(), Result); // Lưu kết quả của từng người chơi vào file log
        }
    }

    void SaveDealerResult(int gameID) {
        if (numberOfPlayersLose > numberOfPlayersWin) {
            LogGameResult(gameID, 0, "Win");
        }
        else if (numberOfPlayersLose < numberOfPlayersWin) {
            LogGameResult(gameID, 0, "Lose");
        }
        else {
            LogGameResult(gameID, 0, "Tie");
        }
    }
    
    void startGame(int gameID) {
        deck.shuffle();
        // Phát bài ban đầu cho tất cả người chơi và dealer
        InitialDeal();
        InitialDeal(); // Phát bài lần 2 để mỗi người chơi và dealer có 2 lá bài ban đầu

        for (auto& player : players) {
            player.PlayerTurn(deck, gameID); // Xử lý lượt chơi của từng người chơi
        }
        dealer.PlayerTurn(deck, gameID); // Xử lý lượt chơi của dealer

        // Xác định kết quả của trò chơi
        Result(gameID);
        // Lưu kết quả của dealer
        SaveDealerResult(gameID);
    }

    ~Game() {}
};

int main() {
    remove("LogTurn.csv"); // Xoá file log cũ nếu tồn tại để bắt đầu một phiên chơi mới
    remove("LogGameResult.csv");
    srand(time(0)); // Khởi tạo seed cho hàm rand() để đảm bảo tính ngẫu nhiên trong mỗi lần chạy chương trình
    int gameID = 1; // Khởi tạo game ID để theo dõi từng game một cách riêng biệt trong file log
    int numberOfGames;
    cout << "Enter the number of simulated games: ";
    cin >> numberOfGames;
    for (int i = 0; i < numberOfGames; ++i) {
        int randomNumplayers = rand() % 7 + 1; // Ngẫu nhiên số lượng người chơi từ 1 đến 7 cho mỗi game
        Game game(randomNumplayers); // Tạo một trò chơi mới với số lượng người chơi ngẫu nhiên
        game.startGame(gameID); // Bắt đầu trò chơi
        gameID++; // Tăng ID trò chơi cho trò chơi tiếp theo
    }
    cout << "End of program!" << endl;
    return 0;
}