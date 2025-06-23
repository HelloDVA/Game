
#include<string>

#define BOARD_SIZE 15

class Connection;

class Game{
	public:
		Game(Connection *player1);
		Game(Connection *player1, Connection *player2);
		~Game();

		bool GameMove(std::string game_request);
		
		bool GameCheck(int x, int y, int role);

		std::string GameRobot(std::string game_request);

	private:
		Connection *player1_;
		Connection *player2_;
		int board_[BOARD_SIZE][BOARD_SIZE];
		static const int directions_[4][2];
	
};
