#include"acceptor.h"
#include"../network/eventloop.h"
#include"../network/socket.h"
#include"../network/channel.h"
#include"../utils/log.h"

#include<fcntl.h>

#include <iostream>

Acceptor::Acceptor(EventLoop *_loop){
    loop = _loop;

    //initial server_sock. register server_channel
    socket = std::make_unique<Socket>();
    socket -> Create();
    socket -> Bind("127.0.0.1", 8081);
    socket -> Listen();

    //accept not use ThreadPoll and do not use ET
    accept_channel = std::make_unique<Channel>(loop, socket -> getfd(), false);
    std::function<void()> _callback = std::bind(&Acceptor::AcceptConnection,this);
    accept_channel -> setfunction(_callback);
    accept_channel -> EnableRead();
}

Acceptor::~Acceptor(){}

// accept client_sock. use server::connection to create connection.
void Acceptor::AcceptConnection(){
    int client_fd = -1;
    socket -> Accept(client_fd);
    newconnectioncallback(client_fd);
}

void Acceptor::setnewconnectioncallback(std::function<void(int)> _cb){
    newconnectioncallback = _cb;
}
