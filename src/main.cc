
#include "./network/tcpserver.h"
#include "./network/eventloop.h"
#include "./network/inetaddress.h"
#include "./utils/globallogger.h"
#include "./server/gomokuserver.h"
#include "./server/sessionmanager.h"

#include <string>

int main() 
{
    // Initialize globallogger system
    GlobalLogger::Initialize("server");
    
    // Create server address.
    std::string ip = "127.0.0.1";
    int port = 8081;
    InetAddress addr(ip.c_str(), port);
    LOG_INFO("server addr ok");

    // Initialize gomoku server through address and main-loop.
    EventLoop loop;
    GomokuServer gomoku (&loop, addr);
    LOG_INFO("gomoku server addr ok");
    gomoku.Start();
    LOG_INFO("gomoku server start");
    
    // Start main-loop.
    loop.Loop();
    return 0;
}
