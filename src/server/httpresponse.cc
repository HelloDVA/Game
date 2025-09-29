#include <sstream>

#include"httpresponse.h"

const std::map<int, std::string> HttpResponse::status_messages_ = {
    {200, "OK"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {500, "Internal Server Error"}
};

std::string HttpResponse::ToString() const {
    std::ostringstream response;
    response << version_ << " " << status_code_ << " " << status_messages_.at(status_code_) << "\r\n";
    response << "Content-Length" << ": " << body_.length() << "\r\n";
	for (const auto& header : headers_) {
		response << header.first + ": " + header.second + "\r\n";
	}
    response << "\r\n" << body_;
    return response.str();
}


/* HttpResponse& HttpResponse::SetCookie(const std::string& name, const std::string& value, int max_age) { */
/*     std::stringstream ss; */
/*     ss << name << "=" << value << "; Max-Age=" << max_age << "; Path=/"; */
/*     headers_["Set-Cookie"] = ss.str(); */
/*     return *this; */
/* } */

/* HttpResponse& HttpResponse::LoadFile(const std::string& filepath) { */
/*     if (!std::filesystem::exists(filepath)) { */
/*         return SetStatus(404).SetBody("404 Not Found"); */
/*     } */
/*     std::ifstream file(filepath, std::ios::binary); */
/*     std::ostringstream ss; */
/*     ss << file.rdbuf(); */
/*     SetBody(ss.str()); */
/*     SetContentType(GetMimeType(filepath)); */
/*     return *this; */
/* } */

bool HttpResponse::EndsWith(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string HttpResponse::GetMimeType(const std::string& path) {
    if (EndsWith(path, ".html")) return "text/html";
    if (EndsWith(path, ".css")) return "text/css";
    if (EndsWith(path, ".js")) return "application/javascript";
    if (EndsWith(path, ".png")) return "image/png";
    if (EndsWith(path, ".jpg") || EndsWith(path, ".jpeg")) return "image/jpeg";
    if (EndsWith(path, ".ico")) return "image/x-icon";
    return "application/octet-stream";
}



