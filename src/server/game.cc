#include <nlohmann/detail/value_t.hpp>
#include<nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include"game.h"
#include"connection.h"
#include"websocket.h"

using json = nlohmann::json;

const int Game::directions_[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

Game::Game(Connection *player1, Connection *player2){
	player1_ = player1;
	player2_ = player2;
};

Game::Game(Connection* player1){
	player1_ = player1;
	player2_ = nullptr;
}

Game::~Game(){};

// parse the message and then update the game
bool Game::GameMove(std::string game_request){

	json data = json::parse(game_request);
	int role = data["role"];
	int x = data["x"];
	int y = data["y"];

	board_[x][y] = role;
	
	if(GameCheck(x, y, role)){
		player1_->getwebconnection()->DoWrite(game_request); 
		player2_->getwebconnection()->DoWrite(game_request); 
		return true;
	} else{
		player1_->getwebconnection()->DoWrite(game_request); 
		player2_->getwebconnection()->DoWrite(game_request); 

		json response_json;
		response_json["type"] = "end";
		response_json["winner"] = role;
		std::string game_response = response_json.dump();	
		player1_->getwebconnection()->DoWrite(game_response); 
		player2_->getwebconnection()->DoWrite(game_response); 
		return false;
	}
}

bool Game::GameCheck(int x, int y, int role){
	for(int i = 0; i < 4; i ++){
		int count = 1;

		int dx = directions_[i][0];
		int dy = directions_[i][1];

		for(int j = 1; j < 5; j ++){
			int new_x = x + dx * j;
			int new_y = y + dy * j;

			if(new_x >= 0 && new_x < BOARD_SIZE && new_y >= 0 && new_y < BOARD_SIZE && board_[new_x][new_y] == role)
				count ++;
			else 
				break;
		}

		for(int j = 1; j < 5; j ++){
			int new_x = x - dx * j;
			int new_y = y - dy * j;

			if(new_x >= 0 && new_x < BOARD_SIZE && new_y >= 0 && new_y < BOARD_SIZE && board_[new_x][new_y] == role)
				count ++;
			else 
				break;
		}

		if(count >= 5)
			return false;
	}
	return true;
}

std::string Game::GameRobot(std::string game_request){
	bool game_state = true;

	json data = json::parse(game_request);
	int role = data["role"];
	int x = data["x"];
	int y = data["y"];

	board_[x][y] = role;
	
	std::string game_response;
	if(!GameCheck(x, y, role)){
		json response_json;
		response_json["type"] = "end";
		response_json["winner"] = role;
		game_response = response_json.dump();	
		game_state = false;
	} else{
		game_response = game_request;
	}

	//player1_->getwebconnection()->DoWrite(game_response);

	return game_response;
}
