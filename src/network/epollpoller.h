#pragma once

#include <vector>

#include <sys/epoll.h>

#include "poller.h"

class Channel;

// @class EpollPoller 
// @brief Wrapping the epoll.
//
// This class create a epoll.  
// Get ready things and get the channel then return all ready channels.
class EpollPoller : public Poller {

    public:
        EpollPoller();
        ~EpollPoller() override;

        virtual void Poll(int timeout_ms, std::vector<Channel*>& activate_channels) override;
        virtual void RemoveChannel(Channel* channel) override;
        virtual void UpdateChannel(Channel* channel) override;

    private:
        static const int MAX_EVENTS = 1024;
        int epollfd_;
        struct epoll_event* events_;   // keep the active events
};
