#include"../utils/buffer.h"
#include"../utils/log.h"

#include<sstream>

#include"httprequest.h"


HttpRequest::HttpRequest(){}

HttpRequest::~HttpRequest(){}

bool HttpRequest::Parse(Buffer* buffer){
   	if(buffer -> Size() == 0)
    	return false;
		
   	std::istringstream request_stream(buffer -> getbuffer());
   	std::string line;
    std::getline(request_stream, line);

    // process the request line
    std::istringstream line_stream(line);
    line_stream >> method_ >> path_ >> version_;

    /* if(path_ == " " || path_ == "/") */
    /*     path_ = "/login"; */

    // process the request header
    while(std::getline(request_stream, line) && line != "\r"){
        std::istringstream header_stream(line);
        std::string key, value;
        if(std::getline(header_stream, key, ':') && std::getline(header_stream, value)){
            header_[key] = value.substr(value.find_first_not_of(" "));
            if(key == "Upgrade"){
                path_ = "/websocket";
            }
        } 
    }

    //process the request body
    if(method_ == "POST" || method_ == "PUT"){
        auto content_length_it = header_.find("Content-Length");
        if(content_length_it != header_.end()){
            size_t content_length = std::stoul(content_length_it->second);
            body_.resize(content_length);
			char body_buffer[content_length];
            request_stream.read(body_buffer, content_length);
			body_ = body_buffer;
        }
    }

	return true;
}

std::string HttpRequest::getversion(){
	
	return version_;
}

std::string HttpRequest::getpath(){
	return path_;
}

std::string HttpRequest::getmethod(){
	return method_;
}

std::string HttpRequest::getbody(){
	return body_;
}

