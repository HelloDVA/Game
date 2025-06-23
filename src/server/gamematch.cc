#include"unistd.h"

#include"game.h"
#include"gamematch.h"
#include"connection.h"
#include <memory>

GameMatch::GameMatch(){}

GameMatch::~GameMatch(){}

void GameMatch::JoinMatch(Connection *player){
	mutex.lock();
	if(!waiting_queue.empty()){
		Connection *opponent = waiting_queue.front();
		waiting_queue.pop();
		std::shared_ptr<Game> game = std::make_shared<Game>(player, opponent);
		player->BindGame(game, -1);
		opponent->BindGame(game, 1);
	}
	else{
		waiting_queue.push(player);
	}
	mutex.unlock();
}
