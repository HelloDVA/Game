#pragma once

#include "../utils/buffer.h"

#include <memory>
#include <string>
#include <functional>

#include "eventloop.h"
#include "channel.h"
#include "inetaddress.h"

// @class TcpConnection 
// @brief Meaning a tcp connection.
//
// Create channel for self, and set epoll events callback function. 
// Callback function will call the server callback function to finish the business.
// When the connection upgrade websocket close the tcp connection.
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
    public:
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

        using ReadCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
        using WriteCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
        using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
        using ErrorCallback = std::function<void(const TcpConnectionPtr&)>;

        TcpConnection(EventLoop* loop,
                      const std::string& name,
                      int sockfd,
                      const InetAddress& localAddr,
                      const InetAddress& peerAddr);
        ~TcpConnection();

        // After tcpserver::NewConnection run.
        void ConnectEstablished();   
        // After tcpserver::CloseConnection run.
        void ConnectDestroyed();             

        // Server initivative methods, need to use loop to run 
        void Send(const std::string& message); 
        void Send(Buffer* message);
        void Shutdown();                     
        
        // Move the socket to websocket.
        void UpgradeWebSocket();

        bool IsConnected() const { return state_ == kConnected; }
        
        // Get and Set methods.
        EventLoop* getloop() const { return loop_; }
        const std::string& getname() const { return name_; }
        const InetAddress& getlocaladdr() const { return local_addr_; }
        const InetAddress& getpeeraddr() const { return peer_addr_; }
        const int getsockfd() const { return channel_->getfd(); }
        void setreadcallback(ReadCallback cb) { read_callback_ = std::move(cb); }
        void setwritecallback(WriteCallback cb) { write_callback_ = std::move(cb); }
        void setclosecallback(CloseCallback cb) { close_callback_ = std::move(cb); }
        void seterrorcallback(ErrorCallback cb) { error_callback_ = std::move(cb); }

    private:
        enum State { kDisconnected, kConnecting, kConnected, kDisconnecting };
        void setstate(State s) { state_ = s; }
        
        void SendInLoop(const std::string& message);
        void ShutdownInLoop();
        
        // Conection callback for epoll events. 
        void HandleRead();
        void HandleWrite();
        void HandleClose();
        void HandleError(int err);

        // Server callback for connection such as GameServer, LogServer.
        // Get from server and use in HandleMethod()
        ReadCallback read_callback_;
        WriteCallback write_callback_;
        CloseCallback close_callback_;
        ErrorCallback error_callback_;

        EventLoop* loop_;
        std::string name_;
        State state_;
        
        std::unique_ptr<Channel> channel_;
        InetAddress local_addr_;
        InetAddress peer_addr_;
        
        Buffer input_buffer_;        
        Buffer output_buffer_;      

        bool upgrade_;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
