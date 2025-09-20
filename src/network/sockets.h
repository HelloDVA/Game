#pragma once

#include <cstddef>

class InetAddress;

namespace sockets {
    int CreateNonblockingOrDie();
    void Bind(int sockfd, const InetAddress& addr);
    void Connect(const InetAddress& addr);
    void Listen(int sockfd);
    int Accept(int sockfd, InetAddress* addr);
    void Close(int sockfd);
    size_t Write(int sockfd, const void* buf, size_t count);
    int CreateEventFd();
    void SetNonBlocking(int sockfd);
}
