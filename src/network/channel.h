#pragma once
#include <functional>
#include <sys/epoll.h>

#include "eventloop.h"

// @class Channel 
// @brief Wrapping the epoll members.
//
// This class wrap the socket, revents, events and bind them to a loop.  
// This class give some functions to control the channel in epoll.
// When the channel ready, epoll will notice the channel, which will use different callback functions for different epoll events. 
class Channel {
public:
    using EventCallback = std::function<void()>;

    Channel(EventLoop* loop, int fd);
    ~Channel();
    
    void HandleEvent();
    
    // Control functions.
    void EnableET() { events_ |= EPOLLET; Update();}
    void EnableReading() { events_ |= kReadEvent; Update(); }
    void DisableReading() { events_ &= ~kReadEvent; Update(); }
    void EnableWriting() { events_ |= kWriteEvent; Update(); }
    void DisableWriting() { events_ &= ~kWriteEvent; Update(); }
    void DisableAll() { events_ = 0; Update(); }
    void RemoveChannel() { Remove(); }
    bool IsReading() const { return events_ & kReadEvent; }
    bool IsWriting() const { return events_ & kWriteEvent; }
    
    // Set and get methods.
    void setreadcallback(EventCallback cb) { read_callback_ = std::move(cb); }
    void setwritecallback(EventCallback cb) { write_callback_ = std::move(cb); }
    void setclosecallback(EventCallback cb) { close_callback_ = std::move(cb); }
    void setrevents(int events) { revents_ = events; }
    void setstate(int state) { state_ = state; }
    int getstate() const { return state_; }
    int getfd() const { return fd_; }
    int getevents() const { return events_; }

private:
    void Update();
    void Remove();

    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;
    int state_; // 0 exist -1 not exist 

    // Epoll events
    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kCloseEvent;
    static const int kErrorEvent;
    
    // Eepoll events callback functions.
    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback close_callback_;
};

