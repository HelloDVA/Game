
#include"../network/channel.h"
#include"../network/eventloop.h"
#include"../utils/threadpool.h"
#include"../utils/log.h"
#include "../database/connectionpool.h"


#include <boost/asio/io_context.hpp>
#include<functional>
#include <queue>

#include"server.h"
#include"acceptor.h"
#include"connection.h"
#include"gamematch.h"
#include"websocket.h"

#define THREADPOOLSIZE 10

std::queue<Connection*> GameMatch::waiting_queue;
asio::io_context WebSocketSession::ioc_;

// create main_reactor subreactors, Acceptor and ThreadPool 
// use main_reactor create Acceptor. Acceptor will let server get new connections
Server::Server(){
    // crate log
    Log::getlog();
    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "log is created");

	main_reactor = std::make_unique<EventLoop>();
    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "main reactor is created");

    // create acceptor for main_reactor
    acceptor = std::make_unique<Acceptor>(main_reactor.get());
    Log::getlog() -> WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "Acceptor ready");
    std::function<void(int)> cb = std::bind(&Server::ConnectNew, this, std::placeholders::_1);
    acceptor -> setnewconnectioncallback(cb);
    
    // make subreactor
    for(int i = 0; i < THREADPOOLSIZE; i ++){
		std::unique_ptr<EventLoop> subreactor = std::make_unique<EventLoop>();
		sub_reactors_.push_back(std::move(subreactor));
	}
    Log::getlog() -> WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "subreactors ready");
    
    // threadpool
    threadpool = std::make_unique<ThreadPool>(THREADPOOLSIZE);
    Log::getlog() -> WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "ThreadPool ready");

    // databasepool
	std::string host = "localhost";
	std::string user = "root";
	std::string password = "mysql123456";
	std::string dbname = "game";
	int poolsize = 10;
	ConnectionPool::getinstance().init(host, user, password, dbname, poolsize);
    Log::getlog() -> WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "DatabasePool ready");
}

Server::~Server(){
    ConnectionPool::getinstance().ClosePool();
    Log::getlog()->CloseLog();
}

void Server::Start(){
    for(int i = 0; i < sub_reactors_.size(); i ++){
        std::function<void()> func = std::bind(&EventLoop::Loop, sub_reactors_[i].get());
        threadpool -> Add(std::move(func));
	}
    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "server start");
	main_reactor->Loop();
}

// use random strategy to get subreactor  
// subreactor is used to create connection
// add subreactor loop function to threadpool
void Server::ConnectNew(int client_fd){
    // random find a subreactor for new connection
     int number = client_fd % sub_reactors_.size(); 
	 std::unique_ptr<Connection> connection = std::make_unique<Connection>(sub_reactors_[number].get(), client_fd);
     std::function<void(int client_fd)> callback = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
      connection -> setdeletecallback(callback);
	  connections[client_fd] = std::move(connection);
}

void Server::DeleteConnection(int client_fd){
    connections.erase(client_fd);
}
