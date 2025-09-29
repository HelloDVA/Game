
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


    // bench test code
    /* TcpServer server(&loop, addr); */
    /* server.setmessagecallback(Test); */
    /* server.Start(); */
    /* std::cout << "main 33 server start\n"; */
    
    // session test code
    /* SessionManager session_manager; */
    /* std::string user_id = "1729778076"; */    

    /* std::cout << "redis connected" << std::endl; */

    /* std::string session_id = session_manager.CreateSession(user_id); */
    
    /* std::cout << session_id << std::endl; */

    /* nlohmann::json session_data = session_manager.GetSession(session_id); */
    
    /* std::cout << session_data["user_id"] << std::endl; */
}
