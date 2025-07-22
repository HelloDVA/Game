#include"../utils/log.h"

#include <memory>
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

void Epoll::UpdateChannel(std::shared_ptr<Channel> ch) {
    int fd = ch->getfd();
    struct epoll_event ev{};
    ev.events = ch->get_listen_events();
    // not add the shared_ptr count number
    ev.data.ptr = ch.get();

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
    channel_map_[fd] = ch;
}

void Epoll::DeleteChannel(std::shared_ptr<Channel> ch){
	int fd = ch->getfd();	
   	if((epoll_ctl(fd_, EPOLL_CTL_DEL, fd, nullptr)) == -1){
       	Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll delete event error");
    } else{
		ch->set_exist(false);
       	Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "epoll delete event");
	}
    channel_map_.erase(fd);
}

std::vector<std::weak_ptr<Channel>> Epoll::Poll(){
    std::vector<std::weak_ptr<Channel>> active_channels;

    int nfds = epoll_wait(fd_, events_, MAX_EVENTS, -1); 
    if(nfds == -1){
       	Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "epoll wait error");
		return active_channels;
    }

    for (int i = 0; i < nfds; ++i) {
        Channel* channel  = static_cast<Channel*>(events_[i].data.ptr);
        int fd = channel->getfd();
        auto it = channel_map_.find(fd);
        if (it != channel_map_.end()) {
            auto channel_ptr = it->second;
            channel_ptr->set_ready_events(events_[i].events);
            std::weak_ptr<Channel> ch_weak = channel_ptr;
            active_channels.push_back(ch_weak);
        }
    }
     return active_channels;
}
