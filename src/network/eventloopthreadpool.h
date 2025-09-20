#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <string>
#include <functional>

class EventLoop;
class EventLoopThread;

// @class EventLoopThreadPool
// @brief Create all sub-loop threasds and save the corresponding loop.
//
// Create threads according to num_thread and thread initcallback function, then keep them.
// Use GetNextLoop() to get a sub-loop therad.
// When the num_thread is 0, GetNextLoop() will return base_loop for single thread model.
class EventLoopThreadPool {
    public:
        using ThreadInitCallback = std::function<void(EventLoop*)>;

        EventLoopThreadPool(EventLoop* base_loop, const std::string& name, int num_thread);
        ~EventLoopThreadPool();
        
        // Create thread, and use initcallback to process them.
        void Start(const ThreadInitCallback& cb = ThreadInitCallback());
        
        EventLoop* GetNextLoop();  

        void setnumthread(int num_thread) {num_thread_ = num_thread;}

    private:
        EventLoop* base_loop_;
        std::string name_;
        bool started_;
        int num_thread_;

        std::atomic<int> next_;

        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*> loops_;
};




