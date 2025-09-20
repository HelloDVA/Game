
#include<map>
#include <string>
#include <map>
#include <sstream>
#include <ctime>
#include <fstream>

class HttpResponse {
public:
    HttpResponse(); 
    ~HttpResponse(); 

    std::string ToString() const;

    void setstatus(int code) {status_code_ = code;}
    void setversion(const std::string& version) {version_ = version;}
    void setbody(const std::string& body) {body_ = body;}
    void setheader(const std::string& key, const std::string& value) { headers_[key] = value; }
    /* HttpResponse& SetContentType(const std::string& type); */
    /* HttpResponse& SetCookie(const std::string& name, const std::string& value, int max_age = 3600); */
    /* HttpResponse& LoadFile(const std::string& filepath); // 静态文件响应 */
    std::string GetMimeType(const std::string& path);
    bool EndsWith(const std::string& str, const std::string& suffix);

private:
    std::string version_;
    int status_code_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    static const std::map<int, std::string> status_messages_;
};

