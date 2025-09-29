#include <cassert>
#include <cstddef>
#include <memory>
#include <iostream>

#include "eventloopthreadpool.h"
#include "eventloopthread.h"
#include "eventloop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop, const std::string& name, int num_thread)
    : base_loop_(base_loop),
      name_(name),
      started_(false),
      num_thread_(num_thread),
      next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::Start(const ThreadInitCallback& cb) {
    assert(!started_);

    base_loop_->AssertInLoopThread(); 

    started_ = true;

    for (int i = 0; i < num_thread_; i ++) {
       std::string name = "IOthread" + std::to_string(i);
       std::cout << "threadpool 29 " << name << std::endl;
       std::unique_ptr<EventLoopThread> t = std::make_unique<EventLoopThread>(cb, name);
       loops_.push_back(t->StartLoop());
       threads_.push_back(std::move(t));
    }

    if (num_thread_ == 0)
       cb(base_loop_);
}

void EventLoopThreadPool::Stop() {
    if (!started_)
        return;

    for(auto loop : loops_) {
       std::cout << "threadpool 44 " << "close loop" << std::endl;
        loop->RunInLoop([loop]() {
            loop->Quit();
        });
    }
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
  base_loop_->AssertInLoopThread();
  assert(started_);
  
  EventLoop* loop = base_loop_;

  if (!loops_.empty()) {
    loop = loops_[next_];
    next_ ++;
  }
  
  if (static_cast<size_t>(next_) >= loops_.size())
    next_ = 0;

  return loop;
}
