#include"eventloop.h"
#include"channel.h"
#include"epoll.h"

#include <memory>
#include<vector>

EventLoop::EventLoop(){
    epoll = std::make_unique<Epoll>();
}

EventLoop::~EventLoop(){}

// here can use asynchronous to add the power of the server
// reactor use epoll to find active tasks 
// run the active task's HandleEvent 
void EventLoop::Loop() {
    while (true) {
        std::vector<std::weak_ptr<Channel>> channels_active = epoll->Poll();
        int length = channels_active.size();

        for(int i = 0; i < length; i ++){
            if (auto channel_active = channels_active[i].lock())
    			channel_active -> HandleEvent();
		}
    }
}

void EventLoop::UpdateChannel(std::shared_ptr<Channel> ch) {
    epoll->UpdateChannel(ch); 
}

void EventLoop::DeleteChannel(std::shared_ptr<Channel> ch) {
    epoll -> DeleteChannel(ch);
}
