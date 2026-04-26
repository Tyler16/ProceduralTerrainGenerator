#include "ThreadPool.h"

#include <mutex>

ThreadPool::ThreadPool(size_t num_threads) {
    stop_ = false;
    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                InputQueueEntry curr_job;
                {
                    std::unique_lock<std::mutex> lock(input_queue_mutex_);
                    cv_.wait(lock, [this] {
                        return !input_queue_.empty() || stop_;
                    });

                    if (stop_ && input_queue_.empty()) {
                        return;
                    }

                    curr_job = std::move(input_queue_.front());
                    input_queue_.pop();
                }
                curr_job.chunk_ptr->setup(curr_job.chunk_x, curr_job.chunk_z);
                {
                    std::unique_lock<std::mutex> lock(output_queue_mutex_);
                    output_queue_.emplace(std::move(curr_job.chunk_ptr));
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

void ThreadPool::enqueue(std::shared_ptr<Chunk> chunk_ptr,
                         int chunk_x,
                         int chunk_z) {
    InputQueueEntry queue_entry;
    queue_entry.chunk_ptr = std::move(chunk_ptr);
    queue_entry.chunk_x = chunk_x;
    queue_entry.chunk_z = chunk_z;
    {
        std::unique_lock<std::mutex> lock(input_queue_mutex_);
        input_queue_.emplace(queue_entry);
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
