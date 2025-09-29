#include <memory>

#include "acceptor.h"
#include "sockets.h"
#include "inetaddress.h"
#include "channel.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr) 
    : loop_(loop), 
      acceptor_channel_(nullptr), 
      listen_addr_(addr) {
    // Create a nonblock socket for this addr.
    acceptor_fd_ = sockets::CreateNonblockingOrDie();
    sockets::Bind(acceptor_fd_, listen_addr_);
    
    acceptor_channel_ = std::make_unique<Channel>(loop_, acceptor_fd_);
    acceptor_channel_->setreadcallback([this]() {HandleRead();});
}

Acceptor::~Acceptor() {
    loop_->AssertInLoopThread();
    acceptor_channel_->RemoveChannel();
    sockets::Close(acceptor_fd_);
}

void Acceptor::Listen() {
    loop_->AssertInLoopThread();
    sockets::Listen(acceptor_fd_);
    acceptor_channel_->EnableReading();
}

void Acceptor::ListenOff() {
    loop_->AssertInLoopThread();
    acceptor_channel_->DisableAll();
}

void Acceptor::HandleRead() {
    InetAddress peer_addr;

    int connfd = sockets::Accept(acceptor_fd_, &peer_addr);

    sockets::SetNonBlocking(connfd);

    if (newconnectioncallback_) {
        newconnectioncallback_(connfd, peer_addr);
    }
    
}
