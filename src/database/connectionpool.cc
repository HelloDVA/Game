
#include<iostream>
#include <mutex>
#include <mysql/mysql.h>

#include"connectionpool.h"

ConnectionPool& ConnectionPool::getinstance(){
	static ConnectionPool instance;
	return instance;
}


void ConnectionPool::init(std::string &host, std::string &user, std::string &password, std::string &dbname, int poolsize){
	host_ = host;
	user_ = user;
	password_ = password;
	dbname_ = dbname;
	port_ = 3306;
	poolsize_ = poolsize;
	for(int i = 1; i <= poolsize; i ++){
		MYSQL* conn = createNewConnection();
		if(conn)
			pool_.push(conn);
	}
}

MYSQL* ConnectionPool::createNewConnection(){
	MYSQL* conn = mysql_init(nullptr);
	if(!conn){
		std::cerr << "mysql init failed" << std::endl;
		return nullptr;
	}
	conn = mysql_real_connect(conn, host_.c_str(), user_.c_str(), password_.c_str(), dbname_.c_str(), port_, nullptr, 0);
	if(!conn){
		std::cerr << "mysql init failed" << std::endl;
		return nullptr;
	}
	if(mysql_set_character_set(conn, "utf8mb4") != 0){
		std::cerr << "mysql set character failed" << std::endl;
		mysql_close(conn);
		return nullptr;
	}
	return conn;
}

MYSQL* ConnectionPool::GetConnection(){
	{
		std::unique_lock<std::mutex> lock(mtx_);
		cv_.wait(lock, [this](){return !pool_.empty();});
		MYSQL* conn = pool_.front();
		pool_.pop();
		if(mysql_ping(conn) !=0){
			std::cerr << "the prepared mysql connection is useless " << std::endl;
			return nullptr;
		}
		return conn;
	}
}

void ConnectionPool::ReleaseConnection(MYSQL* conn){
	{
		std::lock_guard<std::mutex> lock(mtx_);
		pool_.push(conn);
		cv_.notify_one();
	}
}

void ConnectionPool::ClosePool(){
	{
		std::lock_guard<std::mutex> lock(mtx_);
		while(!pool_.empty()){
			MYSQL* conn = pool_.front();
			pool_.pop();
			mysql_close(conn);
		}
	}
}
