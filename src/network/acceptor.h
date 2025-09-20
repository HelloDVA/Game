#pragma once

#include <memory>
#include <functional>

#include "inetaddress.h"

class EventLoop;
class Channel;
class InetAddress;

// @class Acceptor
// @brief Accept new connections for the server.
//
// This Class get main-loop and address from server. Then create channel for self and register the channel. 
// When the EPOLLIN occurs, HandleRead() will use accept() to get socket fd and peer_addr. Then move them to 
// server callback function to create new connection. 
class Acceptor {
    public:
        using NewConnectionCallback = std::function<void(int sockfd, const InetAddress& peer_addr)>;

        Acceptor(EventLoop* loop, const InetAddress& addr);

        // Cancle the channel events and close the socket fd. 
        ~Acceptor();
        
        // Start listen and enable channel in epoll. 
        void Listen();

        void setnewconnectioncallback(std::function<void(int sockfd, const InetAddress& peer_addr)> callback) {
            newconnectioncallback_ = std::move(callback);
        }

    private:
        void HandleRead();
        
        EventLoop *loop_;
        InetAddress listen_addr_;
        std::unique_ptr<Channel> acceptor_channel_;
        int acceptor_fd_;

        NewConnectionCallback newconnectioncallback_;
};
