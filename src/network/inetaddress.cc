
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "inetaddress.h"

InetAddress::InetAddress() {}

InetAddress::InetAddress(const char *ip, uint16_t port) {
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr_.sin_addr);
}

InetAddress::InetAddress(uint16_t port, bool ipv6) {
    addr_.sin_family = ipv6 ? AF_INET6 : AF_INET;
    addr_.sin_port = htons(port);
    memset(&addr_.sin_addr, 0, sizeof(addr_.sin_addr));
}

InetAddress::~InetAddress() {}

std::string InetAddress::ToIp() const { 
    std::string ip = inet_ntoa(addr_.sin_addr);
    return ip;
}

std::string InetAddress::ToPort() const { 
    std::string port = std::to_string(ntohs(addr_.sin_port));
    return port;
}
