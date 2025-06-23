
#include "../utils/log.h"
#include "../database/database.h"
#include "../database/connectionpool.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<regex>

#include"httpresponse.h"

const std::map<int, std::string> HttpResponse::status_messages_ = {
	{200, "OK"},
	{401, "Account or Password error"},
	{404, "Not Found"},
	{500, "Internal Server Error"}
};

HttpResponse::HttpResponse(){}
HttpResponse::~HttpResponse(){}


void HttpResponse::MakeGetResponse(std::string path, std::string version, std::string body){
	status_code_ = 200;
	version_ = version;
	body_ = body;
    path_ = RESOURCE_DIR + path + ".html";

   	std::ifstream file(path_);
   	if (file.is_open()) {
       	std::ostringstream buffer;
       	buffer << file.rdbuf();
       	body_ = buffer.str();
       	file.close();
   	}else{
		MakeErrorResponse(404, version);		
	}
    status_message_ = status_messages_.at(status_code_);
    content_type_ = "text/html";
    headers_["Connection"] = "keep-alive";
	headers_["Content-Length"] = std::to_string(body_.size()); 
}

void HttpResponse::MakeLoginResponse(std::string path, std::string version, std::string body){
	status_code_ = 200;
	version_ = version;
	body_ = body;
    path_ = RESOURCE_DIR + path + ".html";

	ParseUrl(body_);
	MYSQL* conn = ConnectionPool::getinstance().GetConnection();
	
	std::string username = url_data_["username"];
	std::string userpwd;
	std::string query = "SELECT * FROM user WHERE username = '" + username + "'";

	Database database(conn);
	Database::Result rows_result = database.Query(query);

	if(rows_result.empty()){
		MakeErrorResponse(500, version_);
	}

	Database::Row row = database.Query(query).front();
	if(url_data_["password"] == row["password"]){
    	body_ = "/wzq";
    	status_message_ = status_messages_.at(status_code_);
    	content_type_ = "text/html";
    	headers_["Connection"] = "keep-alive";
		headers_["Content-Length"] = std::to_string(body_.size()); 
	}
	else
		MakeErrorResponse(401, version);		
	database.Close();
}

void HttpResponse::MakeErrorResponse(int status_code, std::string version){
	status_code_ = status_code;
	version_ = version;
	body_ = "";
    status_message_ = status_messages_.at(status_code_);
    content_type_ = "text/html";
    headers_["Connection"] = "keep-alive";
	headers_["Content-Length"] = std::to_string(body_.size()); 
}

void HttpResponse::MakeMessageResponse(std::string message, std::string version){
	status_code_ = 200;
	version_ = version;
    body_ = message; 
    status_message_ = status_messages_.at(status_code_);
    content_type_ = "text/html";
    headers_["Connection"] = "keep-alive";
	headers_["Content-Length"] = std::to_string(body_.size()); 

}

std::string HttpResponse::ToString() const{
	std::string response;
	response += version_ + " " + std::to_string(status_code_) + " " + status_message_ + "\r\n";
	response += "Content-Type: " + content_type_ + "\r\n";
	for (const auto& header : headers_) {
		response += header.first + ": " + header.second + "\r\n";
	}
	response += "\r\n";
	response += body_;
    return response;
}

void HttpResponse::ParseUrl(std::string body){
	//before use map clear it
	url_data_.clear();

	std::regex re("([^&=]+)=([^&=]+)");
	auto words_begin = std::sregex_iterator(body.begin(), body.end(), re);
	auto words_end = std::sregex_iterator();

	// 遍历所有匹配的 key=value 对
	for (auto it = words_begin; it != words_end; ++it) {
		std::string key = it->str(1);  // 解码 key
	    std::string value = it->str(2);  // 解码 value
		url_data_[key] = value;
	}
}

std::string HttpResponse::GenerateSessionId() {
    static std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string session_id;
    for (int i = 0; i < 32; ++i) {
        session_id += chars[rand() % chars.size()];
    }
    return session_id;
}
