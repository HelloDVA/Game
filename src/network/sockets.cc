#include <sys/socket.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <stdexcept>

#include "sockets.h"
#include "inetaddress.h"

int sockets::CreateNonblockingOrDie() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0) {
        perror("Create non-blocking socket failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

int sockets::CreateEventFd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC); 
    if (evtfd < 0)
        throw std::runtime_error("Failed to create eventfd");
    return evtfd;
}

void sockets::Bind(int sockfd, const InetAddress& addr) {
    const struct sockaddr_in &addr_in = addr.getsockaddr();
    int ret = ::bind(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in));
    if (ret < 0) {
        perror("Bind socket failed");
        exit(EXIT_FAILURE);
    }
}

void sockets::Listen(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        perror("Listen socket failed");
        exit(EXIT_FAILURE);
    }
}

void sockets::Connect(const InetAddress& addr) {
    const struct sockaddr_in& addr_in = addr.getsockaddr();
    int ret = ::connect(addr_in.sin_family, (struct sockaddr*)&addr_in, sizeof(addr_in));
    if (ret < 0) {
        perror("Connect socket failed");
        exit(EXIT_FAILURE);
    }
}

int sockets::Accept(int sockfd, InetAddress* addr) {
    struct sockaddr_in addr_in;
    socklen_t addr_len = sizeof(addr_in);

    int client_fd = ::accept(sockfd, (struct sockaddr*)&addr_in, &addr_len);

    if (client_fd < 0) {
        perror("Accept connection failed");
        exit(EXIT_FAILURE);
    }

    addr->setsockaddr(addr_in);

    return client_fd;
}

void sockets::Close(int sockfd) {
    if (close(sockfd) < 0) {
        perror("Close socket failed");
        exit(EXIT_FAILURE);
    }
}

size_t sockets::Write(int sockfd, const void* buf, size_t count) {
    return ::write(sockfd, buf, count); 
}

void sockets::SetNonBlocking(int sockfd) { 
    int state = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL)|O_NONBLOCK);
	if(state == -1){
		perror("Socket set nonblock error");
	}
}

