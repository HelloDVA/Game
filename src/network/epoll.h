#pragma once
#include<vector>
#include<sys/epoll.h>
#include<string.h>

class Channel;

class Epoll
{
    public:
        int fd_;
        struct epoll_event *events_;

    public:
        Epoll();
        ~Epoll();

        std::vector<Channel*> Poll();
        void UpdateChannel(Channel *ch);
		void DeleteChannel(Channel *ch);
};
