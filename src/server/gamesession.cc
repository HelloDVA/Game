
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <memory>

#include "gamesession.h"
#include "websocket.h"

using json = nlohmann::json;

bool GameSession::HandleMove(std::shared_ptr<WebSocketSession> player, int x, int y) {
    int role = 0;
    if (player == player1_)
        role = -1;
    if (player == player2_) 
        role = 1;

    bool game_state = game_->MakeMove(x, y, role);
    
    if (game_state) {
        player1_->DoWrite(MakeMessage("end", x, y, role));
        player2_->DoWrite(MakeMessage("end", x, y, role));
        return false;
    } 
    
    // brodcast the mvoe message, if is ai not send
    player1_->DoWrite(MakeMessage("move", x, y, role));
    player2_->DoWrite(MakeMessage("move", x, y, role));
    return true;
}

bool GameSession::HandleAI(std::shared_ptr<WebSocketSession> player, int x, int y) {
    int role = -1;
    int ai_role = 1;

    bool game_end = game_->MakeMove(x, y, role);
    
    if (game_end) {
        player1_->DoWrite(MakeMessage("end", x, y, role));
        return false;
    } 
    player1_->DoWrite(MakeMessage("move", x, y, role));

    // AI turn.
    std::pair<int, int> ai_move = game_->AIMove(); 
    game_end = game_->MakeMove(ai_move.first, ai_move.second, ai_role);
    
    if (game_end) {
        player1_->DoWrite(MakeMessage("end", x, y, ai_role));
        return false;
    } 

    player1_->DoWrite(MakeMessage("move", ai_move.first, ai_move.second, ai_role));
    return true;
}

std::string GameSession::MakeMessage(std::string type, int x, int y, int role) {
    json response_json; 
    response_json["type"] = type; 
    response_json["x"] = x; 
    response_json["y"] = y; 
    response_json["role"] = role; 
    return response_json.dump(); 
}

