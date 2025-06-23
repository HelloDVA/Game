#pragma once
#include<vector>
#include<sys/epoll.h>
#include<string.h>

class Channel;

class Epoll
{
    public:
        int epollfd;
        struct epoll_event *events;

    public:
        Epoll();
        ~Epoll();

        std::vector<Channel*> Poll();
        void UpdateChannel(Channel *ch);
		void DeleteChannel(Channel *ch);
};
