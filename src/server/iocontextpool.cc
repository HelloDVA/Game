

#include "iocontextpool.h"


void IoContextPool::Start() {
    for (auto& ctx : io_contexts_) {
        // Use & to ensure the same io_context run.
        threads_.emplace_back([ctx](){ ctx->run(); });
    }   
}

void IoContextPool::Stop() {
    
}

boost::asio::io_context& IoContextPool::GetIocontext() {
    auto& ctx = *io_contexts_[next_io_context_];
    next_io_context_ = (next_io_context_ + 1) % io_contexts_.size();
    return ctx;
}
