#pragma once
#include<sys/epoll.h>
#include<functional>

class EventLoop;

class Channel{
    private:
        EventLoop *loop;
        int fd;
        uint32_t events;
        uint32_t revents;
        bool inepoll;
        bool usethreadpool;
        std::function<void()> callback;
        
    public:
        Channel(EventLoop *_loop, int _fd, bool _usethreadpool);
        ~Channel();

        void setevents(uint32_t _events);
        void setinepoll(bool _inepoll);
		void setrevents(uint32_t _revents);

        uint32_t getevents();
        uint32_t getrevents();
		bool getinepoll();
        int getfd();

        void setfunction(std::function<void()> _callback);

        void EnableRead();
        void EnableET();
        void HandleEvent();
};
