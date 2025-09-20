
#include <memory>
#include <unordered_map>
#include <queue>
#include <mutex>

#include "game.h"

class WebSocketSession;
class GameAI;

class GameManager {
    public:
        static std::queue<std::shared_ptr<WebSocketSession>> match_queue;
        static GameManager& Instance();
        
        // game start 
        void JoinMatch(std::shared_ptr<WebSocketSession> player);
        void JoinAIMatch(std::shared_ptr<WebSocketSession> player);
        
        // game running
        void HandlePerson(int room_id, std::shared_ptr<WebSocketSession> player, int x, int y);
        void HandleAI(int room_id);

    private:
        // single pattern
        GameManager() {}
        GameManager(const GameManager&) = delete;
        GameManager& operator=(const GameManager&) = delete;

        struct Room {
            std::shared_ptr<WebSocketSession> player1;
            std::shared_ptr<WebSocketSession> player2;
            Game game;
            bool ai;
        };

        std::unordered_map<int, Room> rooms_;
        void GameEnd(int x, int y, int role, Room& room);

        static std::mutex mutex_;
        static int roomid;
        
//        std::unique_ptr<GameAI> ai_;
};
