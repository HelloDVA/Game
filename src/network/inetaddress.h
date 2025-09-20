#pragma once

#include <netinet/in.h>
#include <string>

// @class InetAddress
// @brief Wrapping the network address and port.
//
// The address can be created through three model.
// First, IP and port.
// Secod, port.
// thrid, struct sockaddr_in.
class InetAddress {
    public:
        InetAddress();
        InetAddress(const char *ip, uint16_t port);
        InetAddress(uint16_t port, bool ipv6 = false);
        InetAddress(struct sockaddr_in &addr) : addr_(addr) {};
        ~InetAddress();

        void setsockaddr(const struct sockaddr_in &addr) { addr_ = addr; };
        const struct sockaddr_in& getsockaddr() const {return addr_;};

        std::string ToPort() const;
        std::string ToIp() const;

    private:
        struct sockaddr_in addr_;
};
