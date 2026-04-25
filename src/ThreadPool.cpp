#include "ThreadPool.h"

#include <mutex>

ThreadPool::ThreadPool(size_t num_threads) {
    stop_ = false;
    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                std::shared_ptr<Chunk> chunk;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    cv_.wait(lock, [this] {
                        return !chunk_queue_.empty() || stop_;
                    });

                    if (stop_ && chunk_queue_.empty()) {
                        return;
                    }

                    chunk = std::move(chunk_queue_.front());
                    chunk_queue_.pop();
                }
                chunk->generateVertices();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    cv_.notify_all();

    for (auto& thread : threads_) {
        thread.join();
    }
}

void ThreadPool::enqueue(std::shared_ptr<Chunk> chunk) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        chunk_queue_.emplace(std::move(chunk));
    }
    cv_.notify_one();
}
