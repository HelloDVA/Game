#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>
#include <string>
#include <iostream>

using tcp = boost::asio::ip::tcp;
namespace http = beast::http;
namespace websocket = boost::beast::websocket;
namespace beast = boost::beast;
namespace net = boost::asio;


class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {

public:
    WebSocketSession(tcp::socket);

    void start(std::string request_data);  // 启动握手 + 读
    void send(const std::string& msg); // 向客户端发送数据
    void do_read();

private:
    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
};

