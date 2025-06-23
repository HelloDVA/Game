#include "connection.h"
#include<queue>
#include<mutex>

class Connection;

class GameMatch{
	public:
		GameMatch();
		~GameMatch();
		void JoinMatch(Connection *player);

	private:
		static std::queue<Connection*> waiting_queue;
		std::mutex mutex;
};


