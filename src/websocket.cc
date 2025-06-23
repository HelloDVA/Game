#include "websocket.h"
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/error.hpp>

WebSocketSession::WebSocketSession(tcp::socket socket) : ws_(socket){}


void WebSocketSession::start(std::string request_data) {
    http::request_parser<http::string_body> parser;
    beast::error_code ec;
    parser.put(net::buffer(request_data), ec);
    http::request<http::string_body> req = parser.release();

    ws_.accept(req);
    std::cout << "websocket connection ok" << std::endl;

    // 异步接受 WebSocket 握手
    /* ws_.async_accept(req ,[self = shared_from_this()](boost::system::error_code ec) { */
    /*     if (ec) { */
    /*         std::cerr << "Handshake failed: " << ec.message() << std::endl; */
    /*         return; */
    /*     } */
    /*     //self->do_read(); */
    /* }); */
}

void WebSocketSession::do_read() {
    ws_.async_read(buffer_, [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
        self->on_read(ec, bytes_transferred);
    });
}

void WebSocketSession::on_read(boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
    if (ec == websocket::error::closed) {
        std::cout << "Connection closed." << std::endl;
        return;
    }

    if (ec) {
        std::cerr << "Read error: " << ec.message() << std::endl;
        return;
    }

    std::string message = beast::buffers_to_string(buffer_.data());
    std::cout << "Received: " << message << std::endl;

    // 清空 buffer，准备下一次读取
    buffer_.consume(buffer_.size());

    // 回显（测试用）
    send(message);

    // 继续读
    do_read();
}

void WebSocketSession::send(const std::string& msg) {
    ws_.text(true); // 文本帧
    ws_.async_write(boost::asio::buffer(msg),
        [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
            self->on_write(ec, bytes_transferred);
        });
}

void WebSocketSession::on_write(boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
    if (ec) {
        std::cerr << "Write error: " << ec.message() << std::endl;
    }
}

