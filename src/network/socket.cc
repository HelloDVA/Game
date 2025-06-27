#include"../utils/log.h"

#include<cstdint>

#include<sys/socket.h> 
#include<assert.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

#include"socket.h" 

Socket::Socket() : fd_(-1){}

Socket::Socket(int fd) : fd_(fd) {}

Socket::~Socket(){
    if(fd_ != -1){
        close(fd_);
        fd_ = -1;
    }
}

RC Socket::Create(){
    assert(fd_ == -1);
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_ == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "Failed to create socket");
        return RC_SOCKET_ERROR;
    }

    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "Create socket success");
    return RC_SUCCESS;
}

RC Socket::SetNonBlocking(){
    int state = fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL)|O_NONBLOCK);
	if(state == -1){
		perror("Socket set nonblock error");
		return RC_SOCKET_ERROR;
	}
	else
		return RC_SUCCESS;
}

RC Socket::Bind(const char *ip, uint16_t port) const{
    assert(fd_ != -1);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int bind_state = bind(fd_, (struct sockaddr*)&addr, sizeof(addr));

    if(bind_state == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "Failed to bind socket");
        return RC_SOCKET_ERROR;
    }
    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "Bind socket success");
    return RC_SUCCESS;
}

RC Socket::Listen(){
    assert(fd_ != -1);

    int listen_state = listen(fd_, SOMAXCONN);

    if(listen_state == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "Failed to listen socket");
        return RC_SOCKET_ERROR;
    }
    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "Listen socket success");
    return RC_SUCCESS;
}

RC Socket::Accept(int &clientfd) {
    assert(fd_ != -1);

    clientfd = accept(fd_, NULL, NULL);

    if(clientfd == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "Failed to accept socket");
        return RC_SOCKET_ERROR;
    } else{
        Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "Accept socket success");
        return RC_SUCCESS;
    }
}

RC Socket::Connect(const char *ip, uint16_t port){
    assert(fd_ != -1);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    int connect_state = connect(fd_, (sockaddr*)&addr, sizeof(addr));

    if(connect_state == -1){
        Log::getlog()->WriteLog(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "Failed to connect socket");
        return RC_SOCKET_ERROR;
    }
    Log::getlog()->WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "Connect socket success");
    return RC_SUCCESS;
}

int Socket::getfd() const {    
    return fd_;
}
