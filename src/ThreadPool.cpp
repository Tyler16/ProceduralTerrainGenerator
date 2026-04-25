#include "ThreadPool.h"

#include <mutex>

ThreadPool::ThreadPool(size_t num_threads) {
    stop_ = false;
    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                std::shared_ptr<Chunk> chunk;
                {
                    std::unique_lock<std::mutex> lock(input_queue_mutex_);
                    cv_.wait(lock, [this] {
                        return !input_queue_.empty() || stop_;
                    });

                    if (stop_ && input_queue_.empty()) {
                        return;
                    }

                    chunk = std::move(input_queue_.front());
                    input_queue_.pop();
                }
                chunk->generateVertices();
                {
                    std::unique_lock<std::mutex> lock(output_queue_mutex_);
                    output_queue_.emplace(std::move(chunk));
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> input_lock(input_queue_mutex_);
        stop_ = true;
    }

    cv_.notify_all();

    for (auto& thread : threads_) {
        thread.join();
    }
}

void ThreadPool::enqueue(std::shared_ptr<Chunk> chunk) {
    {
        std::unique_lock<std::mutex> lock(input_queue_mutex_);
        input_queue_.emplace(std::move(chunk));
    }
    cv_.notify_one();
}

std::shared_ptr<Chunk> ThreadPool::dequeue() {
    std::unique_lock<std::mutex> lock(output_queue_mutex_);
    if (output_queue_.empty()) {
        return nullptr;
    }
    std::shared_ptr<Chunk> res = std::move(output_queue_.front());
    output_queue_.pop();
    return res;
}
