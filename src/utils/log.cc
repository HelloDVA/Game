#include<ctime>
#include<iostream>
#include<iomanip>

#include"log.h"

Log* Log::log = NULL;
std::mutex Log::log_mutex;
std::string Log::log_buffer;

Log::Log(){
    init(LOG_LEVEL_NONE, LOG_TYPE_CONSOLE);
}

void Log::init(LOG_LEVEL log_level, LOG_TYPE log_type){
    log_level_ = log_level;
    log_type_ = log_type;
}

Log* Log::getlog(){
    // make sure there is only one instance of log
    if(log == NULL){
        log_mutex.lock();
        if(log == NULL){
            log = new Log();
        }
        log_mutex.unlock();
    }
    return log;
}

void Log::WriteLog(LOG_LEVEL log_level, const char* file_name, const char* function, int line_number, const char* message){
    if(log_level > log_level_){
		return;
    }

    // get date and time
    std::string current_time = GetCurrentTime();

    // get log level
    std::string log_level_str = GetLevelToString(log_level);

    // construct the log message
    std::ostringstream oss;
    oss << current_time << " [" << log_level_str << "] "
        << file_name << ":" << line_number << " "
        << function << " - " << message;

    std::string log_message = oss.str();

    // according to the log_type to write the log
    if(log_type_ == LOG_TYPE_FILE){
        std::ofstream log_file("log.txt", std::ios::app);
        if(log_file.is_open()){
            log_file << log_message << std::endl;
            log_file.close();
        }
    } else if(log_type_ == LOG_TYPE_CONSOLE){
        std::cout << log_message << std::endl;
    }
}

std::string Log::GetCurrentTime(){
    std::time_t now = std::time(nullptr);
    std::tm* localtm = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(localtm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

const char* Log::GetLevelToString(LOG_LEVEL log_level){
    switch(log_level){
        case LOG_LEVEL_NONE:
            return "NONE";
        case LOG_LEVEL_DEBUG:
            return "DEBUG";
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_WARNING:
            return "WARNING";
        case LOG_LEVEL_ERROR:
            return "ERROR";
        case LOG_LEVEL_FATAL:
            return "FATAL";
        default:
            return "NONE";
    }
}

void Log::CloseLog(){
    if(log != NULL){
        delete log;
        log = NULL;
    }
}