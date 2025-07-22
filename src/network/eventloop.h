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

        void UpdateChannel(std::shared_ptr<Channel> ch);
		void DeleteChannel(std::shared_ptr<Channel> ch);
};
