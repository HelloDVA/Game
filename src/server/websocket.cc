
#include <boost/asio/strand.hpp>

#include "websocket.h"
#include "gamemanager.h"

namespace http = beast::http;
namespace net = boost::asio;

WebSocketSession::WebSocketSession(tcp::socket socket) 
    : ws_(std::move(socket)),
      strand_(net::make_strand(ws_.get_executor())) {}

WebSocketSession::~WebSocketSession() {

}

void WebSocketSession::Start(std::string request_data) {
    http::request_parser<http::string_body> parser;
    beast::error_code ec;

    parser.put(net::buffer(request_data), ec);
    http::request<http::string_body> req = parser.release();

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
    
    GameManager& game_manager = GameManager::Instance();
    game_manager.HandleMessage(message, shared_from_this());

    buffer_.consume(buffer_.size());
    DoRead();
}

void WebSocketSession::DoWrite(std::string msg) {
    std::cout << "write: " << msg << std::endl;

    boost::asio::post(
            strand_, 
            [self = shared_from_this(), msg = std::move(msg)] {
                // When the queue is not empty, there is writing.
                bool write_in_progress = !self->write_msgs_.empty();
                self->write_msgs_.push_back(std::move(msg));
                if (!write_in_progress) {
                    self->DoWriteNext();
                }
            }
    );
}

void WebSocketSession::DoWriteNext() { 
    ws_.text(true); 
    ws_.async_write(
        boost::asio::buffer(write_msgs_.front()),
        boost::asio::bind_executor(
            strand_,
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                self->OnWrite(ec, bytes_transferred);
            }
        )
    );
}

void WebSocketSession::OnWrite(boost::system::error_code ec, std::size_t ) {
    if (ec) {
        std::cerr << "Write error: " << ec.message() << std::endl;
        Close();
    }

    write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
            DoWriteNext();
    }
}

void WebSocketSession::Close() {
    ws_.async_close(websocket::close_code::normal,
    [self = shared_from_this()](boost::system::error_code ec) {
        if (ec) {
            std::cerr << "Close error: " << ec.message() << std::endl;
        }
    });
}
