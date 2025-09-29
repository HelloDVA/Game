#include <cstddef>
#include <cstdio>
#include <memory>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <string>
#include <thread>

class AsyncLogger {
    public:
        AsyncLogger(const std::string& basename, 
                size_t rollSize  = 1024*1024*1024,
                int flushInterval = 3);
        ~AsyncLogger();

        void Start();
        void Stop();
        void Append(const char* data, size_t len);
        void Flush();
    
    private:

        void Log(const char* data, size_t len);

        void ThreadFunc();
        void RollFile();

        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::atomic<bool> running_;
        FILE* fp_;
        const std::string basename_;    // logger name
        std::string basepath_;    // logger path initivative path is build
        const size_t rollSize_;         // file max size
        const int flushInterval_;       

        using Buffer = std::vector<char>;
        using BufferPtr = std::shared_ptr<Buffer>;
        using BufferVector = std::vector<BufferPtr>;
        BufferPtr currentBuffer_;   // first buffer
        BufferPtr nextBuffer_;      // second buffer
        BufferVector buffers_;      //
};


