#include<iostream>
#include<unistd.h>

#include"../utils/log.h"
#include"epoll.h"
#include"channel.h"


#define MAX_EVENTS 100

Epoll::Epoll(){
   epollfd = epoll_create1(0);

   if(epollfd == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll create error");
		exit(1);
   }
   else
        Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll create");

   events = new epoll_event[MAX_EVENTS];
   memset(events, 0, sizeof(events) * MAX_EVENTS);
}

Epoll::~Epoll(){
    if(epollfd == -1)
        close(epollfd);
    delete []events;
}

void Epoll::UpdateChannel(Channel *ch){
    int fd = ch->getfd();
    struct epoll_event ev{};
    ev.events = ch->getevents();
    ev.data.ptr = ch;
    if(!ch->getinepoll()){
        if((epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev)) == -1){
            Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll add event error");
        } else{
            Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll add event");    
            ch -> setinepoll(true);
        }
    } else{
        if((epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev)) == -1){
            Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll mod event error");
        } else{
            Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll mod event");
        }
    }
}

void Epoll::DeleteChannel(Channel *ch){
	if(ch->getinepoll()){
		int fd = ch->getfd();	
    	if((epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr)) == -1){
           	Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll delete event error");
        } else{
			ch->setinepoll(false);
        	Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll delete event");
		}
	}
}

std::vector<Channel*> Epoll::Poll(){

    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1); 
    std::vector<Channel*> channel_active;

	// if epoll gets nothing return
    if(nfds == -1){
            	Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll wait error");
		return channel_active;
    }

    for(int i = 0; i < nfds; i ++){
        Channel *ch = (Channel*) events[i].data.ptr;
        ch->setrevents(events[i].events);
        channel_active.push_back(ch);
    }
    return channel_active;
}
