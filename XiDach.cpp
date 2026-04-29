#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <fstream>
#include <cstdlib>

using namespace std;

void LogTurn(ofstream& outFile ,int gameID, int PlayerID, int turnNumber, string Card, int bScore, string Action, string DrawCard, int aScore) {
    if (outFile.is_open()) {
        outFile << gameID << "," 
                << PlayerID << "," 
                << turnNumber << ",\"" 
                << Card << "\"," 
                << bScore << "," 
                << Action << ",\"" 
                << DrawCard << "\"," 
                << aScore << "\n";
    }
}

void LogGameResult(ofstream& outFile, int gameID, int PlayerID, string Result, int CurrentMoney) {
    if (outFile.is_open()) {
        outFile << gameID << "," 
                << PlayerID << ",\"" 
                << Result << "\"," 
                << CurrentMoney << "\n";
    }
}

void LogCompareResult(ofstream& outFile, int gameID, int PlayerID, string PlayerCard, int PlayerScore, string DealerCard, int DealerScore, string Result) {
    if (outFile.is_open()) {
        outFile << gameID << "," 
                << PlayerID << ",\"" 
                << PlayerCard << "\"," 
                << PlayerScore << ",\"" 
                << DealerCard << "\"," 
                << DealerScore << ",\"" 
                << Result << "\"\n";
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
    int money; // Biến để theo dõi số tiền của người chơi, có thể được sử dụng để đặt cược trong các phiên chơi sau
    int numberOfAces; // Kiểm tra số lượng Aces trong tay để xử lý điểm số chính xác
    int numberOfAcesIs1; // Kiểm tra số lượng Aces đã được tính là 1 để điều chỉnh điểm số khi cần thiết
    string result; // Biến để lưu kết quả của người chơi sau khi so sánh với dealer, có thể là "Win", "Lose" hoặc "Tie"
    bool checkBetter28;
protected:
    string Action; // Lưu hành động của người chơi (Hit hoặc Stand)
public:
    Participant(int id) : ID(id), score(0), money(5000), Action(""), numberOfAces(0), numberOfAcesIs1(0), result(""), checkBetter28(false) {}

    virtual ~Participant() {}

    void SetScore(int s) { score = s; }

    int getID() const { return ID; }

    void setResult(string res) { result = res; }

    void setcheckPoint(bool check) {checkBetter28 = check;}

    bool getCheckBetter28() const {return checkBetter28;}

    string getResult() const { return result; }

    string getAction() const { return Action; }

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
        
        if (getHand()[0].getRank() == "A" && getHand()[1].getRank() == "A") {
                return 3;
        } else if (getScore() == 21 && getHandSize() == 2) {
            return 2;
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

    virtual void MakeDecision(Deck& deck, int gameID, int turnNumber, ofstream& turnFile) = 0;  // Hàm ảo thuần túy để các lớp con triển khai logic quyết định riêng

    void PlayerTurn(Deck& deck, int gameID, ofstream& turnFile) {
        int turnNumber = 1; // Khởi tạo lượt chơi đầu tiên
        string sCard  = getHandString();
        if (SpecialWin() > 0) {
            Action = "Stand"; // Nếu có hand đặc biệt, tự động dừng lại
            // Ghi log cho trường hợp thắng đặc biệt
            if (SpecialWin() == 2) {
                LogTurn(turnFile, gameID, ID, turnNumber, sCard, getScore(), "Xi Dach", "", getScore());
            }
            else if (SpecialWin() == 3) {
                LogTurn(turnFile, gameID, ID, turnNumber, sCard, getScore(), "Xi Ban", "", getScore());
            }
            return;
        }
        while (Action != "Stand" && Action != "Ngu Linh") {
            MakeDecision(deck, gameID, turnNumber, turnFile); // Gọi hàm quyết định của người chơi để thực hiện hành động
            turnNumber++; // Tăng số lượt chơi sau mỗi hành động
        }
    }

    void clearHand() {
        hand.clear();
        score = 0;
        numberOfAces = 0;
        numberOfAcesIs1 = 0;
        Action = "";
        result = "";
        checkBetter28 = false;
    }

    int getMoney() const { return money; }

    void addMoney(int amount) { money += amount; }
    void subtractMoney(int amount) { money -= amount; }

    virtual bool isPenalty() const {
        return false;
    }
};

class Player : public Participant {
public:
    Player(int id) : Participant(id) {}
    virtual void MakeDecision(Deck& deck, int gameID, int turnNumber, ofstream& turnFile) override = 0;
};

class FunkyPlayer : public Player {
public:
    FunkyPlayer(int id) : Player(id) {}
    void MakeDecision(Deck& deck, int gameID, int turnNumber, ofstream& turnFile) override {
        // Logic quyết định của FunkyPlayer
        if (getHandSize() == 5) {
            Action = "Stand"; // FunkyPlayer quyết định dừng lại nếu đã có 5 lá bài trong tay
            if (getScore() <= 21) {
                Action = "Ngu Linh"; // Nếu có 5 lá bài trong tay và điểm số không vượt quá 21, thì đây là một hand đặc biệt "Five Cards"
            }
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), getScore(), Action, "", getScore()); // Ghi log cho trường hợp có 5 lá bài trong tay
            return;
        }
        if (getScore() < 16) {
            int bScore = getScore();
            receiveCard(deck.drawCard());
            Action = "Hit"; // FunkyPlayer quyết định rút thêm bài nếu điểm số dưới 16
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), bScore, Action, getHand().back().getRank() + getHand().back().getSuit()[0], getScore()); // Ghi log cho trường hợp rút thêm bài
        } else {
            Action = "Stand"; // FunkyPlayer quyết định dừng lại nếu điểm số là 16 hoặc cao hơn
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), getScore(), Action, "", getScore()); // Ghi log cho trường hợp dừng lại
        }
    }
};

class OptimalPlayer : public Player {
public:
    OptimalPlayer(int id) : Player(id) {}
    void MakeDecision(Deck& deck, int gameID, int turnNumber, ofstream& turnFile) override {
        // Logic quyết định của OptimalPlayer
        if (getHandSize() == 5) {
            Action = "Stand"; // OptimalPlayer quyết định dừng lại nếu đã có 5 lá bài trong tay
            if (getScore() <= 21) {
                Action = "Ngu Linh";
            }
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), getScore(), Action, "", getScore());
            return;
        }
        if (getScore() < 17) {
            int bScore = getScore();
            receiveCard(deck.drawCard());
            Action = "Hit"; // OptimalPlayer quyết định rút thêm bài nếu điểm số dưới 17
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), bScore, Action, getHand().back().getRank() + getHand().back().getSuit()[0], getScore());
        } else {
            Action = "Stand"; // OptimalPlayer quyết định dừng lại nếu điểm số là 17 hoặc cao hơn
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), getScore(), Action, "", getScore());
        }
    }
};

class RecklessPlayer : public Player {
public:
    RecklessPlayer(int id) : Player(id) {}
    void MakeDecision(Deck& deck, int gameID, int turnNumber, ofstream& turnFile) override {
        // Logic quyết định của RecklessPlayer
        if (getHandSize() == 5) {
            Action = "Stand"; // RecklessPlayer quyết định dừng lại nếu đã có 5 lá bài trong tay
            if (getScore() <= 21) {
                Action = "Ngu Linh";
            }
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), getScore(), Action, "", getScore());
            return;
        }
        if (getScore() < 18) {
            int bScore = getScore();
            receiveCard(deck.drawCard());
            Action = "Hit"; // RecklessPlayer quyết định rút thêm bài nếu điểm số dưới 18
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), bScore, Action, getHand().back().getRank() + getHand().back().getSuit()[0], getScore());
        } else if (getScore() == 18 && getHandSize() == 4) {
            int bScore = getScore();
            receiveCard(deck.drawCard());
            Action = "Hit"; // RecklessPlayer quyết định rút thêm bài nếu điểm số là 18 và đã có 4 lá bài trong tay
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), bScore, Action, getHand().back().getRank() + getHand().back().getSuit()[0], getScore());
            if (getScore() >= 28)
                setcheckPoint(true);
        } else {
            Action = "Stand"; // RecklessPlayer quyết định dừng lại nếu điểm số là 18 và có từ 3 lá trở xuống hoặc điểm số là 19 hoặc cao hơn
            LogTurn(turnFile, gameID, getID(), turnNumber, getHandString(), getScore(), Action, "", getScore());
        }
    }

    bool isPenalty() const override {
        return getCheckBetter28();
    }
};

class Dealer : public Participant {
public:
    Dealer(int id) : Participant(id) {}
    void MakeDecision(Deck& deck, int gameID, int turnNumber, ofstream& turnFile) override {
        // Logic quyết định của dealer dựa trên điểm số hiện tại và số lượng bài trong tay
        if (getHandSize() == 5) {
            Action = "Stand"; // Dealer quyết định dừng lại nếu đã có 5 lá bài trong tay
            if (getScore() <= 21) {
                Action = "Ngu Linh"; // Nếu có 5 lá bài trong tay và điểm số không vượt quá 21, thì đây là một hand đặc biệt "Five Cards"
            }
            LogTurn(turnFile, gameID, 0, turnNumber, getHandString(), getScore(), Action, "", getScore()); // Ghi log cho trường hợp có 5 lá bài trong tay
            return;
        }
        // Rút bài nếu điểm số dưới 15
        if (getScore() < 17) {
            int bScore = getScore();
            receiveCard(deck.drawCard());
            Action = "Hit";
            LogTurn(turnFile, gameID, 0, turnNumber, getHandString(), bScore, Action, getHand().back().getRank() + getHand().back().getSuit()[0], getScore());
        } else {
            Action = "Stand"; // Dealer quyết định dừng lại nếu điểm số là 19 hoặc cao hơn
            LogTurn(turnFile, gameID, 0, turnNumber, getHandString(), getScore(), Action, "", getScore());
        }
    }
};

class Game {
private:
    int gameID;
    Deck deck;
    vector<Player*>& players;
    Player* FirstdebtPlayer;
    Dealer*& dealer;
    int numberOfPlayersWin, numberOfPlayersLose, numberOfPlayersTie;
    ofstream& turnFile;
    ofstream& resultFile;
    ofstream& compareFile;
public:
    Game(int id, vector<Player*>& playerList, Dealer*& dealerPtr, ofstream& turnFileRef, ofstream& resultFileRef, ofstream& compareFileRef) : gameID(id), players(playerList), FirstdebtPlayer(nullptr), dealer(dealerPtr), numberOfPlayersWin(0), numberOfPlayersLose(0), numberOfPlayersTie(0), turnFile(turnFileRef), resultFile(resultFileRef), compareFile(compareFileRef) {}

    void ResetPlayers() {
        dealer->clearHand();
        for (auto player : players) {
            player->clearHand();
        }
    }

    void InitialDeal() {
        for (auto player : players) {
            player->receiveCard(deck.drawCard());
        }
        dealer->receiveCard(deck.drawCard());
    }

    void CheckResult(Dealer*& dealer, Player*& player) {
        int dealerScore = dealer->getScore();
        int playerScore = player->getScore();
        if (player->SpecialWin() > 0 || dealer->SpecialWin() > 0) {
            if (dealer->SpecialWin() > player->SpecialWin()) {
                player->setResult("Lose");
            } else if (dealer->SpecialWin() < player->SpecialWin()) {
                player->setResult("Win");
            } else if (dealer->SpecialWin() == 1 && player->SpecialWin() == 1) {
                if (dealerScore > playerScore) {
                    player->setResult("Win");
                } else if (dealerScore < playerScore) {
                    player->setResult("Lose");
                } else {
                    player->setResult("Tie");
                }
            } else {
                player->setResult("Tie");
            }
        }
        else if (playerScore > 21) {
            if (dealerScore > 21) {
                player->setResult("Tie");
            } else {
                player->setResult("Lose");
            }
        } else if (dealerScore > 21 && playerScore <= 21) {
            player->setResult("Win");
        } else if (playerScore > dealerScore) {
            player->setResult("Win");
        } else if (playerScore < dealerScore) {
            player->setResult("Lose");
        } 
        else {
            player->setResult("Tie");
        }
        if (playerScore >= 28 && FirstdebtPlayer == nullptr) {
            FirstdebtPlayer = player;
        }
        LogCompareResult(compareFile, gameID, player->getID(), player->getHandString(), player->getScore(), dealer->getHandString(), dealer->getScore(), player->getResult()); // Ghi log kết quả so sánh giữa người chơi và dealer
    }

    void ResultAndLog(Dealer*& dealer, Player*& player, Player*& debtPlayer) {
        CheckResult(dealer, player);
        string result = player->getResult();
        if (result == "Win") {
            if (players.size() >= 3) {
                // Nếu ở trong Mode 1 thì player[2] là RecklessPlayer nên có nguy cơ 28 điểm
                // Tuy nhiên, nếu ở Mode 2 thì player[2] không bao giờ có thể rút đến 28 điểm
                if (debtPlayer != nullptr) {
                    player->addMoney(1); // Giả sử mỗi lần thắng người chơi nhận được 1 đơn vị tiền
                    debtPlayer->subtractMoney(1); // RecklessPlayer mất 1 đơn vị tiền mỗi khi người chơi thắng
                }
                else {
                    numberOfPlayersWin++;
                    player->addMoney(1); // Giả sử mỗi lần thắng người chơi nhận được 1 đơn vị tiền
                    dealer->subtractMoney(1); // Dealer mất 1 đơn vị tiền mỗi khi người chơi thắng
                }
            }
            else {
                numberOfPlayersWin++;
                player->addMoney(1); // Giả sử mỗi lần thắng người chơi nhận được 1 đơn vị tiền
                dealer->subtractMoney(1); // Dealer mất 1 đơn vị tiền mỗi khi người chơi thắng
            }
        } else if (result == "Lose") {
            numberOfPlayersLose++;
            player->subtractMoney(1); // Giả sử mỗi lần thua người chơi mất 1 đơn vị tiền
            dealer->addMoney(1); // Dealer nhận được 1 đơn vị tiền mỗi khi người chơi thua
        } else {
            numberOfPlayersTie++;
        }
        LogGameResult(resultFile,gameID, player->getID(), result, player->getMoney()); // Ghi log kết quả của người chơi sau khi so sánh với dealer, bao gồm số tiền hiện tại của người chơi
    }

    void SmartDealerTurn() {
        if (dealer->SpecialWin() >= 2) {
            for (auto player : players) {
                ResultAndLog(dealer, player, FirstdebtPlayer); // Kiểm tra kết quả của từng người chơi với dealer nếu dealer có hand đặc biệt
            }
        }
        else {
            int turnNumber = 1; // Khởi tạo lượt chơi đầu tiên của dealer
            while (dealer->getAction() != "Stand" && dealer->getAction() != "Ngu Linh" && dealer->getHandSize() <= 5) {
                if (dealer->getScore() >= 15) {
                    for (auto& player : players) {
                        if (player->getHandSize() >= 3 && player->getHandSize() <= 4 && player->getResult() == "") {
                            ResultAndLog(dealer, player, FirstdebtPlayer); // Kiểm tra kết quả của từng người chơi với dealer nếu người chơi có từ 3 đến 4 lá bài và chưa có kết quả
                        }
                    }
                }
                dealer->MakeDecision(deck, gameID, turnNumber, turnFile); // Dealer thực hiện lượt chơi dựa trên logic quyết định
                turnNumber++; // Tăng số lượt chơi
            }
        }
    }

    void ResultAndLogAll() {
        for (auto& player : players) {
            if (player->getResult() == "") {
                ResultAndLog(dealer, player, FirstdebtPlayer); // Kiểm tra kết quả của tất cả người chơi với dealer nếu chưa có kết quả
            }
        }
    }

    void SaveDealerResult() {
        if (numberOfPlayersLose > numberOfPlayersWin) {
            LogGameResult(resultFile, gameID, 0, "Win", dealer->getMoney());
        }
        else if (numberOfPlayersLose < numberOfPlayersWin) {
            LogGameResult(resultFile, gameID, 0, "Lose", dealer->getMoney());
        }
        else {
            LogGameResult(resultFile, gameID, 0, "Tie", dealer->getMoney());
        }
    }
    
    void startGame() {
        ResetPlayers(); // Đặt lại trạng thái của người chơi và dealer trước khi bắt đầu trò chơi mới
        deck.shuffle();
        // Phát bài ban đầu cho tất cả người chơi và dealer
        InitialDeal();
        InitialDeal(); // Phát bài lần 2 để mỗi người chơi và dealer có 2 lá bài ban đầu

        for (auto& player : players) {
            player->PlayerTurn(deck, gameID, turnFile); // Xử lý lượt chơi của từng người chơi
        }

        SmartDealerTurn(); // Xử lý lượt chơi của dealer sau khi tất cả người chơi đã hoàn thành lượt của mình

        ResultAndLogAll(); // Kiểm tra kết quả của tất cả người chơi với dealer sau khi dealer hoàn thành lượt chơi

        // Lưu kết quả của dealer
        SaveDealerResult();
    }

    ~Game() {}
};

int main() {
    remove("LogTurn.csv"); // Xoá file log cũ nếu tồn tại để bắt đầu một phiên chơi mới
    remove("LogGameResult.csv");
    remove("LogCompareResult.csv");
    ofstream turnFile("LogTurn.csv", ios::app);
    ofstream resultFile("LogGameResult.csv", ios::app);
    ofstream compareFile("LogCompareResult.csv", ios::app);
    turnFile << "GameID,PlayerID,TurnNumber,CardBeforeAction,ScoreBeforeAction,ActionTaken,CardDrawn,ScoreAfterAction\n";
    resultFile << "GameID,PlayerID,Result,CurrentMoney\n";
    compareFile << "GameID,PlayerID,PlayerCard,PlayerScore,DealerCard,DealerScore,Result\n";
    Dealer* dealer = new Dealer(0); // Tạo một đối tượng dealer với ID 0
    cout << "Chon mo phong theo tinh cach nguoi choi (So 1) hoac mo phong theo so luong nguoi choi (So 2): ";
    int select;
    int numberOfGames;
    vector<Player*> players;
    cin >> select;
    if (select == 1) {
        players.push_back(new FunkyPlayer(1));
        players.push_back(new OptimalPlayer(2));
        players.push_back(new RecklessPlayer(3));
        int gameID = 1; // Khởi tạo game ID để theo dõi từng game một cách riêng biệt trong file log
        cout << "Enter the number of simulated games: ";
        cin >> numberOfGames;
        for (int i = 0; i < numberOfGames; ++i) {
            Game game(gameID, players, dealer, turnFile, resultFile, compareFile); // Tạo một trò chơi mới với số lượng người chơi ngẫu nhiên
            game.startGame(); // Bắt đầu trò chơi
            gameID++; // Tăng ID trò chơi cho trò chơi tiếp theo
        }
    }
    else if (select == 2) {
        int numPlayer;
        cout << "\nNhap so luong nguoi choi: ";
        cin >> numPlayer;
        cout <<"\n";
        int ID = 1;
        cout << "Nguoi choi se co ba tinh cach:\n";
        cout << "1 - Ke Nhat Gan (Press 1)\n";
        cout << "2 - Nguoi Toi Uu (Press 2)\n";
        cout << "3 - Ke Mau Lieu (Press 3)\n";
        for (int i = 0; i < numPlayer; i++) {
            int choose;            
            cout << "Chon tinh cach cho Player " << i+1 << ": ";
            cin >> choose;
            if (choose == 1) {
                players.push_back(new FunkyPlayer(ID++));
            }
            else if (choose == 2) {
                players.push_back(new OptimalPlayer(ID++));
            }
            else if (choose == 3) {
                players.push_back(new RecklessPlayer(ID++));
            }
        }
        int gameID = 1;
        cout << "Enter the number of simulated games: ";
        cin >> numberOfGames;
        for (int i = 0; i < numberOfGames; ++i) {
            Game game(gameID, players, dealer, turnFile, resultFile, compareFile); // Tạo một trò chơi mới với số lượng người chơi ngẫu nhiên
            game.startGame(); // Bắt đầu trò chơi
            gameID++; // Tăng ID trò chơi cho trò chơi tiếp theo
        }
    }
    ofstream configFile("config.txt");
    configFile << select << "\n";
    configFile << numberOfGames;
    configFile.close();
    turnFile.close();
    resultFile.close();
    compareFile.close();
    delete dealer;
    for (auto p : players)
        delete p;
    cout << "End of program!" << endl;
    return 0;
}
