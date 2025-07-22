#include"../network/eventloop.h"
#include"../network/socket.h"
#include"../network/channel.h"
#include"../utils/log.h"

#include<fcntl.h>
#include <memory>

#include"acceptor.h"


//initial server_sock. register server_channel
//accept not use ThreadPoll and do not use ET
Acceptor::Acceptor(EventLoop *_loop){
    loop = _loop;

    socket = std::make_unique<Socket>();
    socket -> Create();
    socket -> Bind("127.0.0.1", 8081);
    socket -> Listen();

    accept_channel = std::make_shared<Channel>(loop, socket -> getfd());
    std::function<void()> callback = std::bind(&Acceptor::AcceptConnection,this);
    accept_channel -> set_read_callback(callback);
    accept_channel -> EnableRead();
    accept_channel->EnableET();
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
