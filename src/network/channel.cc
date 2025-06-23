#include"channel.h"
#include"eventloop.h"

Channel::Channel(EventLoop *_loop, int _fd, bool _usethreadpool){
    loop = _loop;
    fd = _fd;
    events = 0;
    revents = 0;
    inepoll =  false;
    usethreadpool = _usethreadpool;
}

Channel::~Channel(){
	if(inepoll)
		loop->DeleteChannel(this);
}

void Channel::setevents(uint32_t _events){
    events = _events;
}

void Channel::setrevents(uint32_t _revents){
    revents = _revents;
}

int Channel::getfd(){
    return fd;    
}

uint32_t Channel::getevents(){
   return events; 
}

uint32_t Channel::getrevents(){
   return revents; 
}

void Channel::setfunction(std::function<void()> _callback){
    callback = _callback;
}

void Channel::setinepoll(bool _inepoll){
    inepoll = _inepoll;
}

bool Channel::getinepoll(){
    return inepoll;    
}

void Channel::EnableRead(){
   events = EPOLLIN | EPOLLPRI;
   loop -> UpdateChannel(this);    
} 

void Channel::EnableET(){
   events |= EPOLLET;
   loop -> UpdateChannel(this);    
} 

void Channel::HandleEvent(){
        callback();
}

