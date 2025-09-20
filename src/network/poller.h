#pragma once

#include <vector>

class Channel;
class EventLoop;

class Poller {
public:
    virtual ~Poller() = default;
    virtual void Poll(int timeoutMs, std::vector<Channel*>& activeChannels) = 0;
    virtual void UpdateChannel(Channel* channel) = 0;
    virtual void RemoveChannel(Channel* channel) = 0;

    // Factory method to create a default poller instance
    static Poller* NewDefaultPoller(EventLoop* loop);
};
