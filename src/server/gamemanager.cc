#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <cmath>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <atomic>

#include "gamemanager.h"
#include "websocket.h"
#include "gamesession.h"

using json = nlohmann::json;

std::queue<std::shared_ptr<WebSocketSession>> GameManager::match_queue;
std::atomic<int> GameManager::roomid_ {1};
std::mutex GameManager::mutex_;

GameManager& GameManager::Instance() {
    static GameManager game_manager;
    return game_manager;
}

void GameManager::HandleMessage(std::string& message, std::shared_ptr<WebSocketSession> player) {
    std::cout << "read: " << message << std::endl;
	json data = json::parse(message);
    
    if (data["type"] == "match") {
        JoinMatch(player);
    }

    if (data["type"] == "move") {
        int roomid = data["roomid"];
        int x = data["x"];
        int y = data["y"];

        std::shared_lock lock(room_mutex_);
        auto game_session = rooms_.at(roomid); 
        bool game_state = game_session->HandleMove(player, x, y);

        // Game over.
        if (!game_state) {
            std::unique_lock lock(room_mutex_);
            rooms_.erase(roomid);
        }
    }

    if (data["type"] == "robot") {
        JoinAIMatch(player);
    }

    if (data["type"] == "ai") {
        int roomid = data["roomid"];
        int x = data["x"];
        int y = data["y"];
        auto game_session = rooms_[roomid]; 
        bool game_state = game_session->HandleAI(player, x, y);

        // Game over.
        if (!game_state) {
            rooms_.erase(roomid);
        }
    }
}

void GameManager::JoinMatch(std::shared_ptr<WebSocketSession> player) {
    // match_queue needs lock before use
    // empty add new player
    // two players in queue, start a game
    if(match_queue.empty()) {
        std::lock_guard<std::mutex> lock(mutex_);
        match_queue.emplace(player);
    } else {
        // Get the other player from queue.
        std::shared_ptr<WebSocketSession> player_o = nullptr;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            player_o = match_queue.front();
            match_queue.pop();
        }

        { 
            std::unique_lock lock(room_mutex_);
            rooms_[roomid_] = std::make_shared<GameSession>(player, player_o);
        }
        
        // send game information to client
        player->DoWrite(MakeStartMessage(roomid_, -1));
        player_o->DoWrite(MakeStartMessage(roomid_, 1));
        
        // change the room id
        roomid_.fetch_add(1, std::memory_order_relaxed); 
   }
}

void GameManager::JoinAIMatch(std::shared_ptr<WebSocketSession> player) {
    {
        std::unique_lock lock(room_mutex_);
        rooms_[roomid_] = std::make_shared<GameSession>(player, nullptr);
    }

    player->DoWrite(MakeStartMessage(roomid_, -1));

    roomid_.fetch_add(1, std::memory_order_relaxed); 
}

std::string GameManager::MakeStartMessage(int roomid, int role) {
    json response_json; 
    response_json["roomid"] = roomid; 
    response_json["type"] = "start"; 
    response_json["role"] = role; 
    return response_json.dump(); 
}
