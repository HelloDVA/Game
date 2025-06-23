
#include"../network/channel.h"
#include"../network/eventloop.h"
#include"../network/socket.h"
#include"../utils/buffer.h"
#include"../utils/log.h"


#include <boost/asio/io_context.hpp>
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
	//		client_socket->SetNonBlocking();

    connection_channel = std::make_unique<Channel>(loop, _fd, true);
    std::function<void()> cb = std::bind(&Connection::HttpConnection, this); 
    connection_channel -> setfunction(cb);
    connection_channel -> EnableRead();
    connection_channel -> EnableET();
    Log::getlog() -> WriteLog(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, "New Connection ready");

    buffer = std::make_unique<Buffer>();
	request_ = std::make_unique<HttpRequest>();
	response_ = std::make_unique<HttpResponse>();

	match_ = std::make_shared<GameMatch>();
}

Connection::~Connection(){
}

void Connection::HttpConnection(){
	char read_buffer[1024];
	bzero(&read_buffer, sizeof(read_buffer));

	int read_state = read(client_socket->getfd(), &read_buffer, sizeof(read_buffer));

	if(read_state > 0){
		std::cout << std::endl << "test in connection 52\n"  << read_buffer << std::endl;
		buffer->Append(read_buffer, read_state); 
		if(request_->Parse(buffer.get())){
			std::string path = request_->getpath();
			std::string method = request_->getmethod();
			std::string version = request_->getversion();
			std::string body = request_->getbody();
		
			// When user press start button in HTML
			if(path == "/websocket"){
				//asio::io_context ioc;
				/* tcp::socket socket(ioc); */ 
				/* socket.assign(tcp::v4(), client_socket->getfd()); */
				webconnection_ = std::make_shared<WebSocketSession>(client_socket->getfd());
				webconnection_->run(buffer->getbuffer());
				
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

				match_->JoinMatch(this); 
				
				bool game_state = true; 
				// game start
				while (game_state) { 
					std::string game_request = webconnection_->DoRead(); 
					game_state = game_-> GameMove(game_request); 
				} 
			} else {
				if (method == "GET")
					response_->MakeGetResponse(path, version,  body);
				if (path == "/wzq" && method == "POST")
					response_->MakeLoginResponse(path, version, body);

				std::string response_message = response_ -> ToString();
				send(client_socket->getfd(), response_message.c_str(), response_message.size(), 0);
				buffer->Clean();
			}
		} else {
			response_->MakeErrorResponse(404, request_->getversion());
			std::string response_message = response_ -> ToString();
			send(client_socket->getfd(), response_message.c_str(), response_message.size(), 0);
			buffer->Clean();
		}
	}

	if (read_state == 0){
		deletecallback(client_socket->getfd()); 
		std::cout << "client out\n";
	}
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
