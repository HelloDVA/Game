#include <string.h>
#include <unistd.h>

#include <iostream>

#include "channel.h"
#include "epollpoller.h"

EpollPoller::EpollPoller() {
    epollfd_ = epoll_create1(0);
    events_ = new epoll_event[MAX_EVENTS];
    memset(events_, 0, sizeof(epoll_event) * MAX_EVENTS);
}

EpollPoller::~EpollPoller() {
    close(epollfd_);
    delete[] events_;
}

void EpollPoller::Poll(int timeout_ms, std::vector<Channel*>& active_channels) { 
    active_channels.clear();
    
    // get all ready fds
    int numEvents = epoll_wait(epollfd_, events_, MAX_EVENTS, timeout_ms);

    if (numEvents < 0) {
        std::cout << "epoll_wait error" << std::endl;
    }

    for (int i = 0; i < numEvents; i++) { 
        Channel* ch = static_cast<Channel*>(events_[i].data.ptr);
        ch->setrevents(events_[i].events);
        active_channels.push_back(ch);
    }
}

void EpollPoller::UpdateChannel(Channel* channel) { 
    int fd = channel->getfd();
    struct epoll_event ev{};
    ev.events = channel->getevents();
    ev.data.ptr = channel;
    int operation = channel->getstate() < 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    if(epoll_ctl(epollfd_, operation, fd, &ev) < 0) {
        std::cout << "epoll_ctl error" << std::endl;
    }
    channel->setstate(0);
}

void EpollPoller::RemoveChannel(Channel* channel) { 
    int fd = channel->getfd();
    if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) < 0) 
        std::cout << "epoll_ctl delete error" << std::endl;
    channel->setstate(-1);
}
