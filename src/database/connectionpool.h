#pragma once

#include <condition_variable>
#include<mysql/mysql.h>

#include<mutex>
#include<string>
#include<queue>
#include<memory>

class ConnectionPool{
	public:
		static ConnectionPool& getinstance();

		void init(std::string &host, std::string &user, std::string &password, std::string &dbname, int poolsize);
		
		MYSQL* GetConnection();
		void ReleaseConnection(MYSQL* conn);
		void ClosePool();

	private:
		ConnectionPool() = default;
		~ConnectionPool(){}
		ConnectionPool(const ConnectionPool&) = delete;
		ConnectionPool& operator =(const ConnectionPool&) = delete;

		MYSQL* createNewConnection();
		std::string host_;
		std::string user_;
		std::string password_;
		std::string dbname_;
		int port_;

		size_t poolsize_;
		std::queue<MYSQL*> pool_;
		std::mutex mtx_;
		std::condition_variable cv_;
};
