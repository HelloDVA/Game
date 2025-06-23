#pragma once
#include<functional>
#include<memory>

class EventLoop;
class Socket;
class Channel;

class Acceptor{
    private:
        // just use
        EventLoop *loop;

        //use and possess
        std::unique_ptr<Socket> socket;
        std::unique_ptr<Channel> accept_channel;
        
        std::function<void(int)> newconnectioncallback;

    public:
        explicit Acceptor(EventLoop *_loop);
        ~Acceptor();
		
        void AcceptConnection();
        void setnewconnectioncallback(std::function<void(int)> _cb);
};
