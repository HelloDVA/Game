#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>

class EpollPoller;
class Channel;

// @class EventLoop 
// @brief Reactor loop to process and manage channels.
//
// The EventLoop is responsible for two things.
// The first is process ready channels.
// The second is manage all channels. To avoid the channel life conflict, such as
// the between sub-loop thread access and the main-loop closed. All chnnels' 
// operatoions should be made by EventLoop.
class EventLoop {
    public:
        using Functor = std::function<void()>;

        EventLoop();
        ~EventLoop();
        
        // Start loop or end loop.
        void Loop();
        void Quit();

        // All channels' operatoions need to aplly.
        void RunInLoop(Functor cb);

        // If the thread is not the loop thread, use the function for registeration.
        void QueueInLoop(Functor cb);

        // Test the current thread whether is the loop thread.
        bool IsInLoopThread() const;

        // Make sure the action is running in the loop thread.
        void AssertInLoopThread();
        
        // Let others wakeup self.
        void Wakeup();  

        void UpdateChannel(Channel* channel);
        void RemoveChannel(Channel* channel);
    private:
        std::unique_ptr<EpollPoller> poller_;
        std::vector<Channel*> activate_channels_;
        std::thread::id thread_id_;
        bool looping_;
        bool quiting_;
        
        // Control the channel operatoions queue.
        std::mutex mutex_;
        std::vector<Functor> pending_functors_;
        
        // Use for wake up the loop.
        int wakeup_fd_;
        std::unique_ptr<Channel> wakeup_channel_;
        
        // Process channel operatoions.
        void DoPendingFunctors();
        
        // Wakeup EPOLLIN function.
        void HnadleRead();
};
