
#include <string>
#include <map>

class HttpRequest;
class HttpResponse;

class HttpProcess {
    public:
        HttpProcess() = default; 
        ~HttpProcess() = default; 

        HttpResponse Process(const HttpRequest& request);
        
    private:
        HttpResponse HandleGet(const HttpRequest& request);
        HttpResponse HandleLogin(const HttpRequest& request);
        void ParseUser(const std::string& body);
        HttpResponse HandleNone(const HttpRequest& request);

    private:
        std::map<std::string, std::string> user_data_;
};
