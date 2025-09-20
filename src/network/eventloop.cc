#include <memory>
#include <stdexcept>

#include <unistd.h>

#include"eventloop.h"
#include"epollpoller.h"
#include "channel.h"
#include "sockets.h"

EventLoop::EventLoop() 
    : poller_(std::make_unique<EpollPoller>()), 
      wakeup_fd_(sockets::CreateEventFd()),
      thread_id_(std::this_thread::get_id()),
      looping_(false),
      quiting_(false) {
    // Create the wakeup channel and active in epoll.
    wakeup_channel_ = std::make_unique<Channel>(this, wakeup_fd_);
    wakeup_channel_->setreadcallback([this]() {
        HnadleRead();
    });
    wakeup_channel_->EnableReading();
}

EventLoop::~EventLoop(){
    wakeup_channel_->DisableAll();
    wakeup_channel_->RemoveChannel();
    ::close(wakeup_fd_);
}

void EventLoop::Loop() {
    AssertInLoopThread();
    looping_ = true;
    quiting_ = false;

    while (!quiting_) {
        activate_channels_.clear();

        poller_->Poll(-1, activate_channels_);

        // Process network events firstly.
        for (Channel* channel : activate_channels_) {
            channel->HandleEvent();
        }

        // Process pending functors secondly.
        DoPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::DoPendingFunctors() {
    std::vector<Functor> functors;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        functors.swap(pending_functors_);
    }
    
    for (const auto& functor : functors) {
        functor();
    }
}

void EventLoop::Quit() {
    quiting_ = true;
    if (!IsInLoopThread()) {
        Wakeup();  
    }
}

void EventLoop::RunInLoop(Functor cb) {
    // If we're already in the loop thread, run the callback immediately
    // Otherwise, queue it to be run later
    if (IsInLoopThread()) 
        cb();
    else 
        QueueInLoop(std::move(cb));
}

void EventLoop::QueueInLoop(Functor cb) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_functors_.emplace_back(cb);
    }

    if (!IsInLoopThread() || quiting_) {
        Wakeup();
    }
}

bool EventLoop::IsInLoopThread() const {
    return thread_id_ == std::this_thread::get_id();
}

void EventLoop::AssertInLoopThread() {
    if (!IsInLoopThread()) {
        throw std::runtime_error("EventLoop::AssertInLoopThread - not in loop thread");
    }
}

void EventLoop::Wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof one) {
        throw std::runtime_error("EventLoop::Wakeup - write wakeup failed");
    }
}

void EventLoop::UpdateChannel(Channel* channel) {
    AssertInLoopThread();
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
    AssertInLoopThread();
    poller_->RemoveChannel(channel);
}

void EventLoop::HnadleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        throw std::runtime_error("EventLoop::HandleRead - read wakeup failed");
    }
}
