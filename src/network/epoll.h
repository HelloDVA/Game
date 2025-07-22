#pragma once
#include <memory>
#include <unordered_map>
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

        std::vector<std::weak_ptr<Channel>> Poll();
        void UpdateChannel(std::shared_ptr<Channel> ch);
		void DeleteChannel(std::shared_ptr<Channel> ch);

    private:
        std::unordered_map<int, std::shared_ptr<Channel>> channel_map_;
};
