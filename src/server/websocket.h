#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <deque>
#include <memory>
#include <string>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    public:
        WebSocketSession(tcp::socket);
        ~WebSocketSession();

        void Start(std::string request_data);  // 启动握手 + 读
        void DoWrite(std::string msg); // 向客户端发送数据
        void DoRead();
        void Close();

    private:
        void DoWriteNext();
        void OnRead(boost::system::error_code ec, std::size_t bytes_transferred);
        void OnWrite(boost::system::error_code ec, std::size_t bytes_transferred);

        websocket::stream<tcp::socket> ws_;
        boost::asio::any_io_executor strand_;
        std::deque<std::string> write_msgs_;
        beast::flat_buffer buffer_;
};

