#include "connection.h"
#include<queue>
#include<mutex>

class Connection;

class GameMatch{
	public:
		GameMatch();
		~GameMatch();

		// use shared_ptr?
		void JoinMatch(Connection *player);

		// how to cancel the match and move the connection

	private:
		static std::queue<Connection*> waiting_queue;
		std::mutex mutex;
};


