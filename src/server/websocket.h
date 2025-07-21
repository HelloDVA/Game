#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>
#include <execution>
#include <iostream>
#include <string>
#include <unistd.h>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = asio::ip::tcp;


class WebSocketSession {
public:
    WebSocketSession(int client_fd);
    ~WebSocketSession();
    void run(std::string request_data);

    std::string DoRead();
    void DoWrite(std::string msg); 

    void Close();

    static asio::io_context ioc_;
private:
    websocket::stream<tcp::socket> ws_;
    int fd_;
};

