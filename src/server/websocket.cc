
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "websocket.h"
#include "gamemanager.h"

using json = nlohmann::json;

WebSocketSession::WebSocketSession(tcp::socket socket) : ws_(std::move(socket)){}

void WebSocketSession::Start(std::string request_data) {
    http::request_parser<http::string_body> parser;
    beast::error_code ec;

    parser.put(net::buffer(request_data), ec);
    http::request<http::string_body> req = parser.release();

    // 异步接受 WebSocket 握手
    // when accept websocket, move client to the match queue
    ws_.async_accept(req ,[self = shared_from_this()](boost::system::error_code ec) {
        if (ec) {
            std::cerr << "Handshake failed: " << ec.message() << std::endl;
            return;
        }
        self->DoRead();
    });
}

void WebSocketSession::DoRead() {
    std::cout << "websocket.cc 24 waiting message " << std::endl;
    ws_.async_read(buffer_, [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
        self->OnRead(ec, bytes_transferred);
    });
}

void WebSocketSession::OnRead(boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
    // error process
    if (ec == websocket::error::closed) {
        std::cout << "Connection closed." << std::endl;
        return;
    }
    if (ec) {
        std::cerr << "Read error: " << ec.message() << std::endl;
        Close();
        return;
    }
    
    // parse client websocket message
    std::string message = beast::buffers_to_string(buffer_.data());
    std::cout << "read: " << message << std::endl;
	json data = json::parse(message);
    GameManager& game_manager = GameManager::Instance();
    
    // take to process according type
    if (data["type"] == "robot") {
        game_manager.JoinAIMatch(shared_from_this());
    }
    if (data["type"] == "ai") {
        game_manager.HandleAI(data["roomid"]);
    }
    if (data["type"] == "match") {
        game_manager.JoinMatch(shared_from_this());
    }
    if (data["type"] == "move") {
        int roomid = data["roomid"];
        int x = data["x"];
        int y = data["y"];
        game_manager.HandlePerson(roomid, shared_from_this(),  x, y);
    }

    // 清空 buffer，准备下一次读取
    buffer_.consume(buffer_.size());
    DoRead();
}

void WebSocketSession::DoWrite(const std::string& msg) {
    std::cout << "write: " << msg << std::endl;
    ws_.text(true); // 文本帧
    ws_.async_write(boost::asio::buffer(msg),
        [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
            self->OnWrite(ec, bytes_transferred);
        });
}

void WebSocketSession::OnWrite(boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
    if (ec) {
        std::cerr << "Write error: " << ec.message() << std::endl;
        Close();
    }
}

void WebSocketSession::Close() {
    // 优雅关闭 WebSocket 连接
    ws_.async_close(websocket::close_code::normal,
    [self = shared_from_this()](boost::system::error_code ec) {
        if (ec) {
            std::cerr << "Close error: " << ec.message() << std::endl;
        }
    });
}
