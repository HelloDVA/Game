#pragma once
#include <cstdint>
#include <memory>
#include<sys/epoll.h>
#include<functional>

class EventLoop;

class Channel : public std::enable_shared_from_this<Channel>{
    private:
        int fd_;
        EventLoop *loop_;

        bool exist_;

        uint32_t listen_events_;
        uint32_t ready_events_;

        std::function<void()> read_callback_;
        std::function<void()> write_callback_;
        
    public:
        Channel(EventLoop *loop, int fd);
        ~Channel();

        int getfd();

        void set_read_callback(std::function<void()> callback);
        void set_write_callback(std::function<void()> callback);
        void set_ready_events(uint32_t revents);
		void set_listen_events(uint32_t events);
        uint32_t get_listen_events();
		bool get_exist();
        void set_exist(bool _inepoll);

        void EnableRead();
        void EnableWrite();
        void EnableET();
        void Disable();
        
        void HandleEvent();
};
