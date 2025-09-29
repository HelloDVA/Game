
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <queue>
#include <mutex>

class WebSocketSession;
class GameAI;
class GameSession;

// class GameManager
// Create, delete, process a game for two players.
class GameManager {
    public:
        static std::queue<std::shared_ptr<WebSocketSession>> match_queue;

        static GameManager& Instance();

        void HandleMessage(std::string& message, std::shared_ptr<WebSocketSession> palyer);
        
        // Create game. 
        void JoinMatch(std::shared_ptr<WebSocketSession> player);
        void JoinAIMatch(std::shared_ptr<WebSocketSession> player);
        
        // Process game action.
        void HandlePerson(int room_id, std::shared_ptr<WebSocketSession> player, int x, int y);
        //void HandleAI(int room_id);
        
        std::string MakeStartMessage(int roomid, int role);

    private:
        // Single pattern.
        GameManager() {}
        GameManager(const GameManager&) = delete;
        GameManager& operator=(const GameManager&) = delete;
        
        mutable std::shared_mutex room_mutex_;
        std::unordered_map<int, std::shared_ptr<GameSession>> rooms_;

        static std::mutex mutex_;
        static std::atomic<int> roomid_;
};
