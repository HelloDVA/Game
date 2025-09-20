#include "../network/tcpconnection.h"

#include <memory>

class TcpServer;
class EventLoop;
class Buffer;
class InetAddress;
class HttpProcess;
class HttpRequest;
class HttpResponse;
class WebSocketSession;

class GomokuServer {
    public:
        GomokuServer(EventLoop* loop, const InetAddress& addr);
        ~GomokuServer();

        void Start();    

        void OnMessage(const TcpConnectionPtr& conn, Buffer* buffer);
        void OnWebSocketMessage(const TcpConnectionPtr& conn, Buffer* buffer);
        void OnConnection(const TcpConnectionPtr&);
        void OnClose();

    private:
        enum class ConnectionState {
            kwaitingForAuth,
            kAuthenticated,
            kInGame
        };

        std::unique_ptr<TcpServer> server_;
};
