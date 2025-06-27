#include"channel.h"
#include"eventloop.h"

Channel::Channel(EventLoop *loop, int fd){
    loop_ = loop;
    fd_ = fd;
    listen_events_ = 0;
    ready_events_ = 0;
    exist_ =false;
}

Channel::~Channel(){
	loop_->DeleteChannel(this);
}


void Channel::set_listen_events(uint32_t events){
    listen_events_ = events;
}

void Channel::set_ready_events(uint32_t events){
    ready_events_ = events;
}

int Channel::getfd(){
    return fd_;    
}


void Channel::set_read_callback(std::function<void()> callback){
    read_callback_ = callback;
}

void Channel::set_write_callback(std::function<void()> callback){
    write_callback_ = callback;
}

void Channel::set_exist(bool exist){
    exist_ =exist;
}

bool Channel::get_exist(){
    return exist_;    
}

void Channel::EnableRead(){
   listen_events_ = EPOLLIN | EPOLLPRI;
   loop_ -> UpdateChannel(this);    
} 

void Channel::EnableET(){
   listen_events_ |= EPOLLET;
   loop_ -> UpdateChannel(this);    
} 

void Channel::HandleEvent(){
    if (ready_events_ & EPOLLIN) {
        read_callback_();
    }
    if (ready_events_ & EPOLLOUT) {
        write_callback_();
    }
}

uint32_t Channel::get_listen_events(){
    return listen_events_;
}
