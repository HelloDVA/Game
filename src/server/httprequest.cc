
#include "httprequest.h"

#include <sstream>

std::string HttpRequest::GetRequest() {
	std::stringstream ss;
        
    // 1. 序列化请求行
    ss << method_ << " " << path_ << " " << version_ << "\r\n";
        
	// 2. 序列化头部字段
	for (const auto& header : headers_) {
		ss << header.first << ": " << header.second << "\r\n";
	}
	
	// 3. 添加空行（分隔头部和body）
	ss << "\r\n";
	
	// 4. 序列化消息体（如果有）
	if (!body_.empty()) {
		ss << body_;
	}
	
	return ss.str();	
}
