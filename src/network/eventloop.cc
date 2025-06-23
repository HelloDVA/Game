#include"eventloop.h"
#include"channel.h"
#include"epoll.h"

#include<vector>

EventLoop::EventLoop(){
    epoll = std::make_unique<Epoll>();
}

EventLoop::~EventLoop(){}

// here can use asynchronous to add the power of the server
// reactor use epoll to find active tasks 
// run the active task's HandleEvent 
void EventLoop::Loop(){
    while(true){
        std::vector<Channel*> channel_active = epoll->Poll();
        int length = channel_active.size();
        for(int i = 0; i < length; i ++){
            int revents_active = channel_active[i]->getrevents();
			if(revents_active & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
				continue;	
			}
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
