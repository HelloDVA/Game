
#include <cstdlib>
#include <hiredis/hiredis.h>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <stdexcept>

#include "sessionmanager.h"

std::string SessionManager::CreateSession(std::string& user_id, int expire_seconds) {
    // create session id and data
    std::string session_id = generatesessionid();
    nlohmann::json session_data;
    session_data["user_id"] = user_id;
    //session_data["create_time"] = static_cast<>();

    std::cout << "create prepared " << std::endl;
    
    // save session
    std::string key = "session:" + session_id;
    redisReply* reply = (redisReply*)redisCommand(context_, 
        "SET %s %s EX %d", key.c_str(), session_data.dump().c_str(), expire_seconds);

    std::cout << "create ok" << std::endl;

    if (!reply || context_->err) {
        freeReplyObject(reply);
        throw std::runtime_error("Failed to create session");
    }
    
    freeReplyObject(reply);

    return session_id;
}

void SessionManager::DeleteSession(std::string& session_id) {
    std::string key = "session:" + session_id;
    redisReply* reply = (redisReply*)redisCommand(context_, "DEL %s", key.c_str());
        
    if (!reply || context_->err) {
        freeReplyObject(reply);
        throw std::runtime_error("Failed to delete session");
    }
    
    freeReplyObject(reply);
}

nlohmann::json SessionManager::GetSession(std::string& session_id) {
    std::string key = "session:" + session_id;
    redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
        
    if (!reply || reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        throw std::runtime_error("Session not found or expired");
    }
    
    if (reply->type == REDIS_REPLY_ERROR) {
        std::string error = reply->str;
        freeReplyObject(reply);
        throw std::runtime_error("Redis error: " + error);
    }

    // after access ,update session time 
    RenewSession(session_id);
    
    // parse str
    try {
        nlohmann::json session_data = nlohmann::json::parse(reply->str);
        freeReplyObject(reply);
        return session_data;
    } catch (const nlohmann::json::parse_error& e) {
        freeReplyObject(reply);
        throw std::runtime_error("Failed to parse session data: " + std::string(e.what()));
    }
}

void SessionManager::RenewSession(std::string& session_id, int expire_seconds) {
    std::string key = "session:" + session_id;
    redisReply* reply = (redisReply*)redisCommand(context_, "EXPIRE %s %d", key.c_str(), expire_seconds);
        
    if (!reply || reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        throw std::runtime_error("Session not found or expired");
    }

    freeReplyObject(reply);
}

void SessionManager::connect() {
    context_ = redisConnect(host_.c_str(), port_);

    // connect error and  create error process
    if (!context_ || context_->err) {
        if (context_) {
            std::string error = context_->errstr;
            redisFree(context_);
            throw std::runtime_error("Redis connect failed: " + error);
        } else {
            throw std::runtime_error("Redis can not create rediscontext");
        }
    }

    // if has password, need verty
    if (!password_.empty()) {
        redisReply* reply = (redisReply*)redisCommand(context_, 
        "AUTH %s", password_.c_str());
        if (!reply || context_->err) {
            freeReplyObject(reply);
            throw std::runtime_error("Redis authentication failed");
        }
        freeReplyObject(reply);
    }
}

std::string SessionManager::generatesessionid() {
    static const char chars[] = 
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    std::string session_id;
    session_id.reserve(32);

    for (int i = 0; i < 32; ++i) {
        session_id += chars[rand() % (sizeof(chars) - 1)];
    }

    return session_id;
}
