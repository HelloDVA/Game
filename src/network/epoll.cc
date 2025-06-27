#include"../utils/log.h"

#include <sys/epoll.h>
#include<unistd.h>

#include"epoll.h"
#include"channel.h"

#define MAX_EVENTS 1000

Epoll::Epoll(){
   fd_ = epoll_create1(0);

   if(fd_ == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll create error");
		exit(1);
   }
   else
        Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll create");

   events_ = new epoll_event[MAX_EVENTS];
   memset(events_, 0, sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll(){
    if(fd_ == -1)
        close(fd_);
    delete []events_;
}

void Epoll::UpdateChannel(Channel *ch){
    int fd = ch->getfd();
    struct epoll_event ev{};
    ev.events = ch->get_listen_events();
    ev.data.ptr = ch;

    if (!ch->get_exist()) {
        if((epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &ev)) == -1)
            Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll add event error");
        else{
            Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll add event");    
            ch -> set_exist(true);
        }
    } else {
        if((epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &ev)) == -1)
            Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll mod event error");
        else
            Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll mod event");
    }
}

void Epoll::DeleteChannel(Channel *ch){
	int fd = ch->getfd();	
   	if((epoll_ctl(fd_, EPOLL_CTL_DEL, fd, nullptr)) == -1){
       	Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll delete event error");
    } else{
		ch->set_exist(false);
       	Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll delete event");
	}
}

std::vector<Channel*> Epoll::Poll(){

    int nfds = epoll_wait(fd_, events_, MAX_EVENTS, -1); 
    std::vector<Channel*> active_channels;

	// if epoll gets nothing return
    if(nfds == -1){
       	Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll wait error");
		return active_channels;
    }
    for (int i = 0; i < nfds; ++i) {
        Channel *ch = (Channel *)events_[i].data.ptr;
        ch->set_ready_events(events_[i].events);
        active_channels.push_back(ch);
    }
     return active_channels;
}
