#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "Chunk.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
  public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    void enqueue(std::shared_ptr<Chunk> chunk);

  private:
    std::vector<std::thread> threads_;
    std::queue<std::shared_ptr<Chunk>> chunk_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool stop_;
};

#endif
