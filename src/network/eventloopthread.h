#pragma once


#include <condition_variable>
#include <functional>
#include <thread>
#include <string>

class EventLoop;

// @class EventLoopThread 
// @brief Thread for EventLoop, provide thread initivate function.
//
// When StartLoop(), create a EventLoop and run in thread function.
// The StartLoop() will return the loop created in thread.
class EventLoopThread {
    public:
        using ThreadInitCallback = std::function<void(EventLoop*)>;
       
        EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), 
                const std::string &name = std::string());
        ~EventLoopThread();
        
        // create thread and return loop
        EventLoop* StartLoop();

        const std::string& getname() const {return name_;}

    private:
        void ThreadFunc();

        EventLoop* loop_;
        std::string name_;
        bool exiting_;
        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
        ThreadInitCallback callback_;  // initivate the loop before the thread start
};
