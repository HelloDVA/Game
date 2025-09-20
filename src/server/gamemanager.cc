
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <cmath>
#include <memory>
#include <mutex>
#include <queue>

#include "gamemanager.h"
#include "websocket.h"

using json = nlohmann::json;

std::queue<std::shared_ptr<WebSocketSession>> GameManager::match_queue;
int GameManager::roomid = 1;
std::mutex GameManager::mutex_;

GameManager& GameManager::Instance() {
    static GameManager game_manager;
    return game_manager;
}

void GameManager::JoinMatch(std::shared_ptr<WebSocketSession> player) {
    // match_queue needs lock before use
    // empty add new player
    // two players in queue, start a game
    if(match_queue.empty()) {
        std::lock_guard<std::mutex> lock(mutex_);
        match_queue.emplace(player);
    } else {
        std::lock_guard<std::mutex> lock(mutex_);
        std::shared_ptr<WebSocketSession> player_o = match_queue.front();
        match_queue.pop();

        // start game player1 is black, player_o is white.
        Game game = Game();
        Room room = {player, player_o, game};
        room.ai = false;
        rooms_[roomid] = room;
        
        // send game information to client
        json response_json1; 
        response_json1["roomid"] = roomid; 
        response_json1["type"] = "start"; 
        response_json1["role"] = -1; 
        std::string game_response1 = response_json1.dump(); 
        player->DoWrite(game_response1);

        json response_json2; 
        response_json2["roomid"] = roomid; 
        response_json2["type"] = "start"; 
        response_json2["role"] = 1; 
        std::string game_response2 = response_json2.dump(); 
        player->DoWrite(game_response2);
        
        // change the room id
        roomid ++;
   }
}

void GameManager::JoinAIMatch(std::shared_ptr<WebSocketSession> player) {
    // start game player1 is black, player_o is white.
    Game game = Game();
    Room room = {player, nullptr, game};
    room.ai = true;
    rooms_[roomid] = room;

    // send game information to client
    json response_json1; 
    response_json1["roomid"] = roomid; 
    response_json1["type"] = "start"; 
    response_json1["role"] = -1; 
    std::string game_response1 = response_json1.dump(); 
    player->DoWrite(game_response1);
    
    // change the room id for next match
    roomid ++;
}

void GameManager::HandlePerson(int room_id, std::shared_ptr<WebSocketSession> player, int x, int y) {
    // get room
    auto it = rooms_.find(room_id);
    if (it == rooms_.end())
        return;
    Room& room = it->second;
    
    // get role the first player is black, the other is white
    int role = 0;
    if (player == room.player1)
        role = -1;
    if (player == room.player2) 
        role = 1;

    bool game_state = room.game.MakeMove(x, y, role);
    
    if (game_state) {
        GameEnd(x, y, role, room);
        return;
    } 

    std::string response_message;
    json response_json; 
    response_json["type"] = "move"; 
    response_json["x"] = x; 
    response_json["y"] = y; 
    response_json["role"] = role; 
    response_message = response_json.dump(); 
    
    // brodcast the mvoe message, if is ai not send
    if(room.player1 != nullptr)
        room.player1->DoWrite(response_message);
    if(room.player2 != nullptr)
        room.player2->DoWrite(response_message);
}

void GameManager::HandleAI(int room_id) {
    // get room
    auto it = rooms_.find(room_id);
    if (it == rooms_.end())
        return;
    Room& room = it->second;

    // human often is black and move first 
    std::pair<int, int> aimove = room.game.AIMove();
    int role = 1;
    int x = aimove.first;
    int y = aimove.second;
    bool game_state = room.game.MakeMove(x, y, role);
    if (game_state) {
        GameEnd(x, y, role, room);
        return;
    } 
    
    // send move message to client
    std::string response_message;
    json response_json; 
    response_json["type"] = "ai"; 
    response_json["x"] = x; 
    response_json["y"] = y; 
    response_json["role"] = role; 
    response_message = response_json.dump(); 
    room.player1->DoWrite(response_message);
}

void GameManager::GameEnd(int x, int y, int role, Room& room) {
        std::string response_message;
        json response_json; 
        response_json["type"] = "end"; 
        response_json["role"] = role; 
        response_json["x"] = x; 
        response_json["y"] = y; 
        response_message = response_json.dump(); 
        if (room.ai)
            room.player1->DoWrite(response_message);
        else {
            room.player1->DoWrite(response_message);
            room.player2->DoWrite(response_message);
        }
}
