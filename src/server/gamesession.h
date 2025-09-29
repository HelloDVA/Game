

#include <memory>

#include "game.h"

class WebSocketSession;

// class GameSession
//
// This means a game, handle the move action.
class GameSession {
    public: 
        GameSession(std::shared_ptr<WebSocketSession> player1, std::shared_ptr<WebSocketSession> player2)
            : player1_(player1),
              player2_(player2),
              game_(std::make_unique<Game>(Game())) {}

        ~GameSession() {}

        bool HandleMove(std::shared_ptr<WebSocketSession> player, int x, int y);
        bool HandleAI(std::shared_ptr<WebSocketSession> player, int x, int y);
        
        std::string MakeMessage(std::string type, int x, int y, int role);

    private:
        std::unique_ptr<Game> game_;
        std::shared_ptr<WebSocketSession> player1_;
        std::shared_ptr<WebSocketSession> player2_;
};
