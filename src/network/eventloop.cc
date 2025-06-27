#include"eventloop.h"
#include"channel.h"
#include"epoll.h"

#include<vector>
#include<thread>
#include <iostream>

EventLoop::EventLoop(){
    epoll = std::make_unique<Epoll>();
}

EventLoop::~EventLoop(){}

// here can use asynchronous to add the power of the server
// reactor use epoll to find active tasks 
// run the active task's HandleEvent 
void EventLoop::Loop() {
    while (true) {
        std::vector<Channel*> channel_active = epoll->Poll();
        std::cout << "Working Current thread id: " << std::this_thread::get_id() << std::endl; 
        int length = channel_active.size();
        for(int i = 0; i < length; i ++){
			channel_active[i] -> HandleEvent();
		}
    }
}

void EventLoop::UpdateChannel(Channel *ch){
   epoll->UpdateChannel(ch); 
}

void EventLoop::DeleteChannel(Channel *ch){
	epoll -> DeleteChannel(ch);
}
