
#include"../network/channel.h"
#include"../network/eventloop.h"
#include"../network/socket.h"
#include"../utils/buffer.h"
#include"../utils/log.h"


#include <boost/asio/io_context.hpp>
#include <cerrno>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include<unistd.h>
#include<sys/socket.h>

#include<iostream>
#include <memory>
#include <string>

#include"connection.h"
#include"httprequest.h"
#include"httpresponse.h"
#include"gamematch.h"
#include"game.h"
#include"websocket.h"

using json = nlohmann::json;


Connection::Connection(EventLoop *_loop, int _fd){
    loop = _loop;

	client_socket = std::make_unique<Socket>(_fd);
	client_socket->SetNonBlocking();

    connection_channel = std::make_unique<Channel>(loop, _fd);

    std::function<void()> read_cb = std::bind(&Connection::HttpRead, this); 
    std::function<void()> write_cb = std::bind(&Connection::HttpWrite, this); 
    connection_channel -> set_read_callback(read_cb);
    connection_channel -> set_write_callback(read_cb);

    connection_channel -> EnableRead();
    connection_channel -> EnableET();

    Log::getlog() -> WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "New Connection ready");

    buffer_ = std::make_unique<Buffer>();
	request_ = std::make_unique<HttpRequest>();
	response_ = std::make_unique<HttpResponse>();

	match_ = std::make_unique<GameMatch>();
}

Connection::~Connection(){
	
}

void Connection::HttpRead() {
	char read_buffer[1024];
    while (true) {
		bzero(&read_buffer, sizeof(read_buffer));
		int read_state = read(client_socket->getfd(), &read_buffer, sizeof(read_buffer));

        if (read_state > 0) {
			buffer_->Append(read_buffer, read_state); 
//			std::cout << buffer_->Cstr() << std::endl;
		} else if (read_state == -1 && errno ==EINTR) {
			continue;	
        } else if (read_state == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			std::cout << "data read is ok +++++++++++++++++++++++++++" << std::endl;
			HttpProcess();
			buffer_->Clean();
			break;
        } else if (read_state == 0) {
			deletecallback(client_socket->getfd()); 
			std::cout << "client out\n";
			break;
        } else {
        	deletecallback(client_socket->getfd());
			break;
        } 
   	}

        /* if (read_state > 0) { */
			/* buffer_->Append(read_buffer, read_state); */ 
        /* } else if (read_state == 0) { */
			/* deletecallback(client_socket->getfd()); */ 
			/* std::cout << "client out\n"; */
			/* break; */
        /* } else { */
        /*     if (errno == EAGAIN || errno == EWOULDBLOCK) { */
			/* //	HttpProcess(); */
				/* buffer_->Clean(); */
        /* 	} else if (errno == EINTR) { */
        /*     	// 被信号中断，继续读取 */
        /*     	continue; */
        /* 	} else { */
        /*     	// 出错 */
        /*     	//std::cout << "read error" << std::endl; */
        /* 	} */
    	/* } */
}

void Connection::HttpProcess(){
//	std::cout << std::endl << "test in connection 52\n"  << buffer->Cstr() << std::endl;
	if (request_->Parse(buffer_.get())) {
		std::string path = request_->getpath();
		std::string method = request_->getmethod();
		std::string version = request_->getversion();
		std::string body = request_->getbody();
		std::string response_message;

		// websocket process 
		if (path == "/websocket") 
			WebSocketProcess();

		if (path == "/login" && method == "POST") {
			response_->MakeLoginResponse(path, version, body);
			response_message = response_->ToString();
		}

		if (method == "GET") {
			response_->MakeGetResponse(path, version,  body);
			response_message = response_->ToString();
		}
		send(client_socket->getfd(), response_message.c_str(), response_message.size(), 0);
	} else {
		response_->MakeErrorResponse(404, request_->getversion());
		std::string response_message = response_ -> ToString();
		send(client_socket->getfd(), response_message.c_str(), response_message.size(), 0);
	}
			{					
			// play with robot
			/* std::shared_ptr<Game> game = std::make_shared<Game>(this); */  
			/* BindGame(game, 0); */ 	
			/* bool game_state = true; */
			/* json response_json; */
			/* response_json["type"] = "start"; */
			/* response_json["role"] = game_role_; */
			/* std::string game_response = response_json.dump(); */
			/* webconnection_->DoWrite(game_response); */
			/* while(game_state){ */
			/* 	std::cout << "waiting for message\n"; */
			/* 	std::string game_request = webconnection_->DoRead(); */
			/* 	game_response = game_->GameRobot(game_request); */
			/* 	game_state = game_->GameCheck(); */
			/* 	webconnection_->DoWrite(game_response); */
			/* } */
			}
}

void Connection::WebSocketProcess(){
	//asio::io_context ioc;
	/* tcp::socket socket(ioc); */ 
	/* socket.assign(tcp::v4(), client_socket->getfd()); */
	webconnection_ = std::make_shared<WebSocketSession>(client_socket->getfd());
	webconnection_->run(buffer_->getbuffer());

	match_->JoinMatch(this); 
					
	// game start
 	bool game_state = true; 

	// game start
	while (game_state) { 
		std::string game_request = webconnection_->DoRead(); 
		if (auto game = game_.lock()) {  // 尝试提升为 shared_ptr
			// 对象仍然存在，可以使用
			game_state = game_-> GameMove(game_request);
		} else {
			webconnection_->Close();
		} 
	} 
	webconnection_->Close();
}

void Connection::HttpWrite(){

}

void Connection::BindGame(std::shared_ptr<Game> game, int game_role){
	game_ = game;
	game_role_ = game_role;
	json response_json; 
	response_json["type"] = "start"; 
	response_json["role"] = game_role_; 
	std::string game_response = response_json.dump(); 
	webconnection_->DoWrite(game_response); 
}

void Connection::setdeletecallback(std::function<void(int fd)> callback){
		deletecallback = callback;
}

WebSocketSession* Connection::getwebconnection(){
	return webconnection_.get();
}
