
#include <hiredis/hiredis.h>

#include<nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <stdexcept>
#include <string>

class SessionManager { 
    public:
        SessionManager(const std::string& host = "127.0.0.1", 
                    int port = 6379, 
                    const std::string& password = "") 
            : host_(host),
              port_(port),
              password_(password) {
            connect();
        }

        ~SessionManager() {
            if (!context_)
                redisFree(context_);
        }

        // create Session
        std::string CreateSession(std::string& user_id, int expire_seconds = 3600);
        // delete session
        void DeleteSession(std::string& session_id);
        // renew session 
        void RenewSession(std::string& session_id, int expire_seconds = 3600);
        // find session
        nlohmann::json GetSession(std::string& session_id);
    private:
        redisContext* context_;   
        std::string host_;
        int port_;
        std::string password_;

        void connect();
        std::string generatesessionid();
};
