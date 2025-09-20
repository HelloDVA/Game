#include "asynclogger.h"

#include <cstdint>
#include <memory>

class AsyncLogger;

// singel pattern
class GlobalLogger {
  public:
    GlobalLogger(const GlobalLogger&) = delete;  
    GlobalLogger operator=(const GlobalLogger&) = delete;

    // initialize logger_  
    static void Initialize(const std::string& name) {
        instance().logger_.reset(new AsyncLogger(name));
        instance().logger_->Start();
    }

    // close logger_
    static void Shutdown() {
        if (instance().logger_) 
            instance().logger_->Stop();
    }
    
    // method_
    static void Append(const char* msg, size_t len) {
        if (instance().logger_)
            instance().logger_->Append(msg, len);
    }
  
  private:
    GlobalLogger() = default;
    ~GlobalLogger() {
        Shutdown();
    }
    // create singel instance for use
    static GlobalLogger& instance() {
        static GlobalLogger logger;
        return logger;
    }


    std::unique_ptr<AsyncLogger> logger_;
};

// 宏定义方便使用
#define LOG_INFO(format, ...) \
    do { \
        char buf[4096]; \
        int len = snprintf(buf, sizeof buf, "[INFO] " format "\n", ##__VA_ARGS__); \
        GlobalLogger::Append(buf, len); \
    } while (0)

#define LOG_ERROR(format, ...) \
    do { \
        char buf[4096]; \
        int len = snprintf(buf, sizeof buf, "[ERROR] " format "\n", ##__VA_ARGS__); \
        GlobalLogger::Append(buf, len); \
    } while (0)

