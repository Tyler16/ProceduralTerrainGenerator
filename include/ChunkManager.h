#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "Camera.h"
#include "Chunk.h"
#include "ChunkPool.h"
#include "GlobalConstants.h"
#include "HeightGenerator.h"
#include "ShaderProgram.h"
#include "ThreadPool.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <unordered_map>

class ChunkManager {

  public:
    ChunkManager(int seed, ShaderProgram shader);
    void render(float current_frame);
    void update(double curr_time);

  private:
    static constexpr int RENDER_DIST = 16;
    static constexpr int CLEANUP_DIST = 17;
    static constexpr size_t NUM_POOL_CHUNKS = (2 * CLEANUP_DIST + 1) * (2 * CLEANUP_DIST + 1);
    size_t NUM_THREADS = std::thread::hardware_concurrency() - 1;
    static constexpr float MAX_HEIGHT = 100.0f;
    static constexpr double CLEANUP_INTERVAL = 1.0f;
    static constexpr int SET_DATA_LIMIT = 2;
    static constexpr float WORLD_RENDER_DIST = Constants::Chunks::CHUNK_SIZE * RENDER_DIST;
    static constexpr float FOG_MIN = WORLD_RENDER_DIST * 0.4f;
    static constexpr float FOG_MAX = WORLD_RENDER_DIST - (Constants::Chunks::CHUNK_SIZE * 0.5f);

    std::unordered_map<uint64_t, std::shared_ptr<Chunk>> active_chunks_;
    ThreadPool thread_pool_;
    ChunkPool chunk_pool_;
    HeightGenerator height_generator_;
    ShaderProgram shader_;
    Camera& camera_;
    int last_cam_x_;
    int last_cam_z_;
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

