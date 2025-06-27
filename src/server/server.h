#pragma once
#include<unordered_map>
#include<vector>
#include<memory>
#include<utility>

class EventLoop;
class Acceptor;
class Connection;
class Socket;
class ThreadPool;

class Server
{
    private:
        // use and possess
        std::unique_ptr<EventLoop> main_reactor;
        std::unique_ptr<Acceptor> acceptor;
        std::unique_ptr<ThreadPool> threadpool;

        std::vector<std::unique_ptr<EventLoop>> sub_reactors_;
        std::unordered_map<int, std::unique_ptr<Connection>> connections;

    public:
        Server();
        ~Server();
		void Start();
        void ConnectNew(int client_fd);
        void DeleteConnection(int client_fd);
};

