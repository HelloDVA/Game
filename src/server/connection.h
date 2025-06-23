#pragma once

#include <boost/asio/io_context.hpp>
#include<functional>
#include<memory>

class EventLoop;
class Socket;
class Channel;
class Buffer;
class HttpRequest;
class HttpResponse;
class GameMatch;
class Game;
class WebSocketSession;

enum ConnectionState{
    HTTP, WEBSOCKET
};

class Connection{

    private:
        //just use
        EventLoop *loop;

        //use and possess
		std::unique_ptr<Socket> client_socket;
        std::unique_ptr<Channel> connection_channel;
		std::unique_ptr<HttpRequest> request_;
		std::unique_ptr<HttpResponse> response_;
        std::unique_ptr<Buffer> buffer;

        std::shared_ptr<WebSocketSession> webconnection_;
		std::shared_ptr<GameMatch> match_;
        int game_role_;
        std::shared_ptr<Game> game_;
        std::function<void(int fd)> deletecallback;

        ConnectionState state = HTTP;

	public:
        Connection(EventLoop *_loop, int _fd);
        ~Connection();

        void setdeletecallback(std::function<void(int fd)> callback);
		
        void Echo(int fd);
		void HttpConnection();
        void BindGame(std::shared_ptr<Game> game, int game_role);

        WebSocketSession* getwebconnection();
};
