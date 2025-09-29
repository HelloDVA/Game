#include "../network/tcpserver.h"
#include "../network/eventloop.h"
#include "../network/inetaddress.h"
#include <boost/asio/io_context.hpp>

#include <memory>

#include "gomokuserver.h"
#include "httpprocess.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "websocket.h"
#include "httpparser.h"
#include "iocontextpool.h"

GomokuServer::GomokuServer(EventLoop* loop, const InetAddress& addr) 
    : server_(std::make_unique<TcpServer>(loop, addr)),
      context_pool_(std::make_unique<IoContextPool>(5)){
    server_->setmessagecallback([this](const TcpConnectionPtr& conn, Buffer* buffer) {OnMessage(conn, buffer);});
    server_->setconnectioncallback([this](const TcpConnectionPtr& conn) {OnConnection(conn);});
}

GomokuServer::~GomokuServer() {

}

void GomokuServer::Start() {
    server_->Start();
    context_pool_->Start();
}

void GomokuServer::OnMessage(const TcpConnectionPtr& conn, Buffer* buffer) {
    HttpRequest request;
    HttpParser parser;
    HttpProcess process;
    HttpResponse response;

    std::string message(buffer->Peek(), buffer->ReadableBytes());
    std::cout << "gomokuserver.cc 32 message: " << message << std::endl;
    
    // Request parse, test websocket or http. 
    if (parser.Parse(*buffer, request)) {
        if (request.getheader("Upgrade") == "websocket") {
            // move sockfd to websocket from epoll and close connection
            std::string message = request.GetRequest();
            int sockfd = conn->getsockfd();
            conn->UpgradeWebSocket(); 

            // Get an io_context from pool.
            auto& io_context = context_pool_->GetIocontext();

            //net::io_context io_context;
            tcp::socket socket(io_context);
            socket.assign(tcp::v4(), sockfd);

            std::shared_ptr<WebSocketSession> websocket = std::make_shared<WebSocketSession>(std::move(socket));    
            websocket->Start(message); 
            return;
        } else {
            response = process.Process(request);
            conn->Send(response.ToString());
        }
    }
}
