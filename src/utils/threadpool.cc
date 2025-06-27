#include"threadpool.h"

ThreadPool::ThreadPool(int size) : stop(false){
    // threads keep testing task queue. If task queue ready, thread will execute task. 
    for(int i = 0;i < size;i ++){
        threads.emplace_back(std::thread([this](){
                        while(true){
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(task_mutex);

                                cv.wait(lock, [this]{
                                        return stop || !tasks.empty();
                                    });
                                
                                if(stop && tasks.empty()) return;
                                task = tasks.front();
                                tasks.pop();
                            }
                            task(); 
                        }
                    }));
    }
}

ThreadPool::~ThreadPool(){
    {
          std::unique_lock<std::mutex> lock(task_mutex);
          stop = true;
    }
    cv.notify_all();
    for(std::thread &thread : threads){
        if(thread.joinable())
            thread.join();
    } 
}

//void ThreadPool::add(std::function<void()> func)
//{
//    std::unique_lock<std::mutex> lock(task_mutex);
//    if(stop)   
//        throw std::runtime_error("ThreadPool aleday stop, can't add task any more");
//    tasks.emplace(func);
//    cv.notify_one();
//}
