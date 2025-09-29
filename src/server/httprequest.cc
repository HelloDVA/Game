
#include "httprequest.h"

#include <sstream>

std::string HttpRequest::GetRequest() {
	std::stringstream ss;
        
    ss << method_ << " " << path_ << " " << version_ << "\r\n";
        
	for (const auto& header : headers_) {
		ss << header.first << ": " << header.second << "\r\n";
	}
	
	ss << "\r\n";
	
	if (!body_.empty()) {
		ss << body_;
	}
	
	return ss.str();	
}
