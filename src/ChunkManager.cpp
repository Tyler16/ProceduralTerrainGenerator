#include "ChunkManager.h"

#include "BufferPool.h"
#include "GlobalConstants.h"
#include "ThreadPool.h"
#include <cstdlib>
#include <memory>

ChunkManager::ChunkManager(int seed) :
  thread_pool_(NUM_THREADS),
  buffer_pool_(NUM_BUFFERS),
  height_generator_(seed, MAX_HEIGHT),
  last_cleanup_time_(0.0) {}

uint64_t ChunkManager::chunkPosToKey(int chunk_x, int chunk_z) {
    return (static_cast<uint64_t>(chunk_x) << 32)
           | (static_cast<uint64_t>(chunk_z) & 0xFFFFFFFFL);
}

std::pair<int, int> ChunkManager::chunkKeyToPos(uint64_t key) {
    return {
        static_cast<int>(static_cast<int64_t>(key) >> 32),
        static_cast<int>(static_cast<int32_t>(key & 0xFFFFFFFF))
    };
}

std::pair<int, int> ChunkManager::posToChunkPos(glm::vec3 pos) {
    return {
        static_cast<int>(std::floor(pos[0] / Constants::Chunks::CHUNK_SIZE)),
        static_cast<int>(std::floor(pos[2] / Constants::Chunks::CHUNK_SIZE))
    };
}

int ChunkManager::getDistance(int x1, int z1, int x2, int z2) {
    return std::max(abs(x1 - x2), abs(z1 - z2));
}

bool ChunkManager::chunkInBounds(int camera_x, int camera_z,
                                 int chunk_x, int chunk_z,
                                 int threshold_dist) {
    return getDistance(camera_x, camera_z, chunk_x, chunk_z) <= threshold_dist;
}

void ChunkManager::cleanup(int camera_x, int camera_z) {
    for (auto it = active_chunks_.begin(); it != active_chunks_.end();) {
        auto [chunk_x, chunk_z] = it->second->getPos();

        if (!chunkInBounds(camera_x, camera_z, chunk_x, chunk_z, CLEANUP_DIST)) {
            if (it->second->isReady()) {
                buffer_pool_.release(it->second->getBufferSet());
            }
            it = active_chunks_.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ChunkManager::checkFinishedChunks() {
    std::shared_ptr<Chunk> curr_chunk;
    int num_set = 0;
    while (buffer_pool_.hasBuffer()
           && num_set < SET_DATA_LIMIT
           && (curr_chunk = thread_pool_.dequeue()) != nullptr) {
        auto [chunk_x, chunk_z] = curr_chunk->getPos();
        uint64_t key = chunkPosToKey(chunk_x, chunk_z);

        auto it = active_chunks_.find(key);
        if (it == active_chunks_.end() || it->second != curr_chunk) {
            continue;
        }

        BufferSet buffers = buffer_pool_.acquire();

        curr_chunk->setBufferData(buffers);
        ++num_set;
    }
}

void ChunkManager::createChunks(int camera_x, int camera_z) {
    int min_x = camera_x - RENDER_DIST;
    int min_z = camera_z - RENDER_DIST;
    int max_x = camera_x + RENDER_DIST;
    int max_z = camera_z + RENDER_DIST;

    for (int curr_z = min_z; curr_z <= max_z; ++curr_z) {
        for (int curr_x = min_x; curr_x <= max_x; ++curr_x) {
            uint64_t key = chunkPosToKey(curr_x, curr_z);
           
            auto [it, inserted] = active_chunks_.try_emplace(key);
            if (inserted) {
                it->second = std::make_shared<Chunk>(height_generator_);
                thread_pool_.enqueue(it->second, curr_x, curr_z);
            }
        }
    }
}

void ChunkManager::update(glm::vec3 camera_pos, double curr_time) {
    auto [camera_x, camera_z] = posToChunkPos(camera_pos); 

    createChunks(camera_x, camera_z);
    checkFinishedChunks();
    
    if (curr_time - last_cleanup_time_ > CLEANUP_INTERVAL) {
        cleanup(camera_x, camera_z);
        last_cleanup_time_ = curr_time;
    }
}

void ChunkManager::render() {
    for (const auto& chunk_pair : active_chunks_) {
        chunk_pair.second->draw();
    }
    glBindVertexArray(0);
}

