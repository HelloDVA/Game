#pragma once
#include<sys/epoll.h>

#include"common.h"

class Socket{
    private:
        int fd;
    public:
        Socket();
        Socket(int _fd);
        ~Socket();

        int getfd() const;

        RC Create();
        RC Bind(const char *ip, uint16_t port) const;
        RC Listen();
        RC Accept(int &clientfd);
        RC Connect(const char *ip, uint16_t port);
        RC SetNonBlocking();
        bool IsNonBlocking();
};

