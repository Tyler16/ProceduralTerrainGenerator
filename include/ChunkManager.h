#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "Chunk.h"
#include "ChunkPool.h"
#include "HeightGenerator.h"
#include "ThreadPool.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>

class ChunkManager {

  public:
    ChunkManager(int seed);
    void render();
    void update(glm::vec3 camera_pos, double curr_time);

  private:
    static constexpr int RENDER_DIST = 5;
    static constexpr int CLEANUP_DIST = 6;
    static constexpr size_t NUM_BUFFERS = 100;
    static constexpr size_t NUM_POOL_CHUNKS = 200;
    size_t NUM_THREADS = std::thread::hardware_concurrency() - 1;
    static constexpr float MAX_HEIGHT = 100.0f;
    static constexpr double CLEANUP_INTERVAL = 1.0f;
    static constexpr int SET_DATA_LIMIT = 2;

    std::unordered_map<uint64_t, std::shared_ptr<Chunk>> active_chunks_;
    ThreadPool thread_pool_;
    ChunkPool chunk_pool_;
    HeightGenerator height_generator_;
    double last_cleanup_time_;

    uint64_t chunkPosToKey(int chunk_x, int chunk_z);
    std::pair<int, int> chunkKeyToPos(uint64_t);
    std::pair<int, int> posToChunkPos(glm::vec3 pos);
    int getDistance(int x1, int z1, int x2, int z2);
    bool chunkInBounds(int camera_x, int camera_z,
                       int chunk_x, int chunk_z, int threshold_dist);
    void cleanup(int camera_x, int camera_z);
    void checkFinishedChunks();
    void createChunks(int camera_x, int camera_z);
};

#endif

