#pragma once
#include<functional>
#include<memory>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop
{
    private:
        std::unique_ptr<Epoll> epoll;

    public:
        EventLoop();
        ~EventLoop();

        void Loop();

        void UpdateChannel(Channel* ch);
		void DeleteChannel(Channel* ch);
};
