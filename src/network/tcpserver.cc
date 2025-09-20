#include <unistd.h>

#include <cassert>
#include <iostream>
#include <memory>

#include "tcpserver.h"
#include "acceptor.h"
#include "eventloop.h"
#include "eventloopthreadpool.h"
#include "inetaddress.h"
#include "tcpconnection.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& addr) 
    : loop_(loop),
      name_("TcpServer"),
      started_(false),
      local_addr_(addr),
      acceptor_(std::make_unique<Acceptor>(loop, addr)),
      thread_pool_(std::make_unique<EventLoopThreadPool>(loop, "IOthreadpool", 5)) {
    // Bind new connection callback for acceptor. 
    std::function<void(int, const InetAddress&)> cb = std::bind(&TcpServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2);
    acceptor_->setnewconnectioncallback(cb);    
}

TcpServer::~TcpServer() {
    loop_->AssertInLoopThread();

    for (auto it : connections_) {
        TcpConnection::TcpConnectionPtr conn = it.second;
        conn->getloop()->RunInLoop([conn](){
                conn->ConnectDestroyed();
            });
    }
}

void TcpServer::Start() {
    if (started_)
        return;
    started_ = true;

    thread_pool_->Start();

    loop_->RunInLoop([this](){
            acceptor_->Listen();
    });
}

void TcpServer::NewConnection(int sockfd, const InetAddress& peer_addr) {
    loop_->AssertInLoopThread();    

    std::string conn_name = peer_addr.ToIp() + peer_addr.ToPort() + "->" + local_addr_.ToIp() + local_addr_.ToIp();
    
    // Choose a loop from thread pool for the new connection.
    EventLoop* loop = thread_pool_->GetNextLoop();
    TcpConnection::TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop, conn_name, sockfd, local_addr_, peer_addr);
    connections_[conn_name] = conn;
    
    // Bind message and close callback function for new connection.
    conn->setclosecallback([this](const TcpConnection::TcpConnectionPtr &conn) {
            CloseConnection(conn);
    });
    conn->setreadcallback(message_callback_);

    loop->RunInLoop([conn]() {
            conn->ConnectEstablished();
    });
}

void TcpServer::CloseConnection(const TcpConnection::TcpConnectionPtr& conn) {
    loop_->RunInLoop([this, conn]() {
            CloseConnectionInLoop(conn);
    });
}

void TcpServer::CloseConnectionInLoop(const TcpConnection::TcpConnectionPtr& conn) {
   loop_->AssertInLoopThread(); 

   std::cout << "tcp server 79 close connection" << std::endl;

   std::string name = conn->getname();
   ssize_t n = connections_.erase(name);
   (void)n;
   assert(n == 1);
   EventLoop* loop = conn->getloop();
   loop->RunInLoop([conn]() {
            conn->ConnectDestroyed();
    });
}

