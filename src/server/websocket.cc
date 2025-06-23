
#include"websocket.h"
#include <boost/beast/websocket/rfc6455.hpp>

WebSocketSession::WebSocketSession(int client_fd)
        : ws_(tcp::socket(ioc_)) {
        fd_ = client_fd;
        // 接管原始 socket fd
        beast::get_lowest_layer(ws_).assign(tcp::v4(), client_fd);
}

void WebSocketSession::run(std::string request_data) {
        try {
            http::request_parser<http::string_body> parser;
            beast::error_code ec;
            parser.put(net::buffer(request_data), ec);
            http::request<http::string_body> req = parser.release();

            // 完成 WebSocket 握手
            ws_.accept(req);
            std::cout << "[Session] WebSocket handshake completed.\n";
        } catch (const std::exception& e) {
            std::cerr << "Session error: " << e.what() << "\n";
        }
}

std::string WebSocketSession::DoRead(){
    beast::flat_buffer buffer;
    try{
        ws_.read(buffer);
    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << "\n";
    }
    std::string msg = beast::buffers_to_string(buffer.data());
    std::cout << "get " << msg << "websocket 34 " << fd_ << std::endl;
    return msg;
}

void WebSocketSession::DoWrite(std::string msg){
    std::cout << "send "<< msg << "websocket 38 " << fd_ << std::endl;
    try{
        ws_.write(asio::buffer(msg));
    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << "\n";
    }
}

void WebSocketSession::Close(){
    ws_.close(beast::websocket::close_code::normal);
    std::cout << "the websocket is close normal" << std::endl;
}
