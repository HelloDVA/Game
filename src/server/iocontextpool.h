#include <boost/asio/io_context.hpp>

#include <cstddef>
#include <vector>
#include <memory>
#include <thread>

// class::IoContextPool
// This an io_context pool. 
class IoContextPool {
    public: 
        explicit IoContextPool(std::size_t size) : next_io_context_(0) {
            for (std::size_t i = 0; i < size; ++i) {
                auto ctx = std::make_shared<boost::asio::io_context>();
                work_.emplace_back(boost::asio::make_work_guard(*ctx));
                io_contexts_.push_back(ctx);
            }
        }
        
        void Start();
        void Stop();

        boost::asio::io_context& GetIocontext();

    private:
        std::vector<std::shared_ptr<boost::asio::io_context>> io_contexts_;
        std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_;
        std::vector<std::thread> threads_;
        std::size_t next_io_context_;
};
