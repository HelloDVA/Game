#include <cerrno>
#include <cstddef>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#include "tcpconnection.h"
#include "sockets.h"
#include "../utils/globallogger.h"

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const InetAddress& local_addr,
                             const InetAddress& peer_addr)
    : loop_(loop),
      name_(name),
      state_(kConnecting),
      channel_(std::make_unique<Channel>(loop, sockfd)),
      local_addr_(local_addr),
      peer_addr_(peer_addr),
      upgrade_(false) {
    // 注册各种事件回调
    channel_->setreadcallback([this]() { this->HandleRead(); });
    channel_->setwritecallback([this]() { this->HandleWrite(); });
    channel_->setclosecallback([this]() { this->HandleClose(); });
}

TcpConnection::~TcpConnection() {
    if (!upgrade_) {
        int sockfd = channel_->getfd();
        sockets::Close(sockfd);
    }
}

void TcpConnection::ConnectEstablished() {
    loop_->AssertInLoopThread();
    assert(state_ == kConnecting);

    setstate(kConnected);
    channel_->EnableReading();  
    channel_->EnableET();
}

void TcpConnection::ConnectDestroyed() {
    loop_->AssertInLoopThread();
    if (state_ == kConnected) {
        setstate(kDisconnected);
    }
    
    // All is done, close in loop.
    channel_->RemoveChannel();
}

void TcpConnection::HandleRead() {
    loop_->AssertInLoopThread();
    
    // Use buffer to make sure the http request is completed.
    int saved_errno;

    while (true) {
        ssize_t n = input_buffer_.ReadFd(channel_->getfd(), &saved_errno);
        if (n > 0) {
            if (read_callback_) {
                read_callback_(shared_from_this(), &input_buffer_);
            }
        } else if (n == 0) {
            HandleClose();
            break;
        } else {
            if (saved_errno == EAGAIN || saved_errno == EWOULDBLOCK) { 
                break;
            } else if (saved_errno == EINTR) {
                // Single stop.
                continue;
            } else {
                LOG_ERROR("TcpConnection::HandleRead error");
                HandleError(saved_errno);
                break;
            }
        }
    }

    /* ssize_t n = input_buffer_.ReadFd(channel_->getfd(), &saved_errno); */
    /* std::cout << "tcpconnection 58 read bytes:  " << n <<std::endl; */
    /* if (n > 0) { */
    /*     if (read_callback_) { */
    /*         read_callback_(shared_from_this(), &input_buffer_); */
    /*     } */
    /* } else if (n == 0) { */
    /*     // Client close the connection. */
    /*     HandleClose(); */
    /* } else if (saved_errno == EAGAIN || saved_errno == EWOULDBLOCK) { */
    /* } else { */
    /*     std::cerr << "TcpConnection::HandleRead error" << std::endl; */
    /*     HandleError(saved_errno); */
    /* } */
}

void TcpConnection::HandleWrite() {
    loop_->AssertInLoopThread();

    if (channel_->IsWriting()) {
        ssize_t n = sockets::Write(channel_->getfd(), 
                                  output_buffer_.Peek(), 
                                  output_buffer_.ReadableBytes());
        if (n > 0) {
            output_buffer_.Retrieve(n);
            
            // All data has been send, close write.
            if (output_buffer_.ReadableBytes() == 0) {
                channel_->DisableWriting();
                
                // If the client has closed the connection, close after the send.
                if (state_ == kDisconnecting) 
                    ShutdownInLoop();
            }
        } else {
            LOG_ERROR("TcpConnection::HandleWrite error");
        }
    }
}

void TcpConnection::HandleClose() {
    loop_->AssertInLoopThread();

    if (state_ == kDisconnected) 
        return;
    
    setstate(kDisconnected);
    channel_->DisableAll();

    // Protect the connection close safely.
    TcpConnectionPtr guardThis(shared_from_this());

    // Call server to delete information about the connection.
    if (close_callback_) {
        close_callback_(guardThis);
    }
}

void TcpConnection::HandleError(int err) {
    std::string log_info = "TcpConnection::HandleError [" + name_ + "] - SO_ERROR = " + std::to_string(err);
    LOG_ERROR("%s", log_info.c_str());
    // Close the connection.
    HandleClose(); 
}

void TcpConnection::Send(const std::string& message) {
    if (state_ == kConnected) {
        if (loop_->IsInLoopThread()) 
            SendInLoop(message);
        else 
            loop_->RunInLoop([this, msg = message]() {
                    SendInLoop(msg);
            });
    }
}

void TcpConnection::SendInLoop(const std::string& message) {
    // Make sure the connection is ok.
    if (state_ != kConnected) 
        return;

    ssize_t nwrote = 0;
    size_t remaining = message.size();
    bool fault_error = false;
        
    // When there is no data waiting to send, send directly.
    if (!channel_->IsWriting() && output_buffer_.ReadableBytes() == 0) {
        nwrote = ::write(channel_->getfd(), message.data(), message.size());
        if (nwrote >= 0) {
            remaining -= nwrote;
            // All data has been send.
            if (remaining == 0) {
            }
        }
        else {
            // Serious error when write data.
            if (errno != EAGAIN || errno != EWOULDBLOCK) {
                    fault_error = true;
            }
        }
    }
    
    // Register write.
    if (!fault_error && remaining > 0) {
        output_buffer_.Append(message.data() + nwrote, message.size() - nwrote);
        if (!channel_->IsWriting())
            channel_->EnableWriting();
    }
    
   // append data to buffer
   if (remaining > 0) {
        output_buffer_.Append(message.data() + nwrote, remaining);
        if (!channel_->IsWriting())
            channel_->EnableWriting();
   }
}

void TcpConnection::Shutdown() {
    if (state_ == kConnected) {
        setstate(kDisconnecting);
        loop_->RunInLoop([this]() {
                ShutdownInLoop();
        });
    }
}

void TcpConnection::ShutdownInLoop() {
    loop_->AssertInLoopThread();
    if (!channel_->IsWriting()) {
       ::shutdown(channel_->getfd(), SHUT_WR); 
    }
    HandleClose();
}

void TcpConnection::UpgradeWebSocket() {
    upgrade_ = true;
    HandleClose();
}

