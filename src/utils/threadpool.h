#pragma once
#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<future>


class ThreadPool{

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        
        std::mutex task_mutex;
        std::condition_variable cv;
        bool stop;

    public:
        ThreadPool(int size = 10);
        ~ThreadPool();
//        void add(std::function<void()> func);
        template <typename F, typename... Args>
        auto Add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
        
};

template <typename F, typename... Args>
auto ThreadPool::Add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
     std::future<return_type> result = task -> get_future();

     //submit (future)function into ThreadPool tasks 
     {
        std::unique_lock<std::mutex> lock(task_mutex);
        if(stop)
            throw std::runtime_error("ThreadPool is error");
        tasks.emplace([task](){(*task)();});
     }
     cv.notify_one();
     return result;
}
