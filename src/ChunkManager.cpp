#include "ChunkManager.h"

#include "Camera.h"
#include "ChunkPool.h"
#include "Frustrum.h"
#include "GlobalConstants.h"
#include "ThreadPool.h"

#include <cstdlib>
#include <memory>

ChunkManager::ChunkManager(int seed, ShaderProgram shader) :
    thread_pool_(NUM_THREADS),
    height_generator_(seed, MAX_HEIGHT),
    chunk_pool_(NUM_POOL_CHUNKS, height_generator_),
    shader_(shader),
    camera_(Camera::getInstance()),
    last_cleanup_time_(0.0),
    last_cam_x_(200),
    last_cam_z_(200) {}

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
            if (it->second->getStatus() == ChunkStatus::ACTIVE) {
                chunk_pool_.returnChunk(std::move(it->second));
            }
            else {
                it->second->setStatus(ChunkStatus::RETIRED);
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
    while (num_set < SET_DATA_LIMIT
           && (curr_chunk = thread_pool_.dequeue()) != nullptr) {
        auto [chunk_x, chunk_z] = curr_chunk->getPos();
        uint64_t key = chunkPosToKey(chunk_x, chunk_z);

        auto it = active_chunks_.find(key);

        bool is_stale = (it == active_chunks_.end()) ||
                        (it->second != curr_chunk) ||
                        (curr_chunk->getStatus() == ChunkStatus::RETIRED);

        if (is_stale) {
            chunk_pool_.returnChunk(std::move(curr_chunk));
            continue;
        } 

        curr_chunk->setBufferData();
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
                std::shared_ptr<Chunk> new_chunk = std::move(chunk_pool_.getChunk());
                if (new_chunk == nullptr) {
                    return;
                }
                it->second = std::move(new_chunk);
                thread_pool_.enqueue(it->second, curr_x, curr_z);
            }
        }
    }
}

void ChunkManager::update(glm::vec3 camera_pos, double curr_time) {
    auto [camera_x, camera_z] = posToChunkPos(camera_.getPosition()); 

    checkFinishedChunks();

    if (last_cam_x_ ==  camera_x && last_cam_z_ == camera_z) {
        return;
    }
    createChunks(camera_x, camera_z);
    
    if (curr_time - last_cleanup_time_ > CLEANUP_INTERVAL) {
        cleanup(camera_x, camera_z);
        last_cleanup_time_ = curr_time;
    }
    last_cam_x_ = camera_x;
    last_cam_z_ = camera_z;
}

void ChunkManager::render(float current_frame) {

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
    glm::mat4 view = camera_.getViewMatrix();
    glm::mat4 vp_matrix = Constants::Matrices::PROJECTION * view;
    Frustrum frustrum = Frustrum(vp_matrix);

    shader_.use();
    shader_.setMat4("projection", Constants::Matrices::PROJECTION);
    shader_.setMat4("view", view);
    shader_.setMat4("model", model);
    shader_.setMat3("normalMatrix", normal_matrix);

    shader_.setVec3("lightPos", Constants::Matrices::LIGHT_POS);
    shader_.setVec3("viewPos", camera_.getPosition());
    shader_.setVec3("fogColor", Constants::Colors::FOG);
    shader_.setFloat("fogMinDist", FOG_MIN);
    shader_.setFloat("fogMaxDist", FOG_MAX);
    shader_.setFloat("uTime", current_frame);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (const auto& [key, chunk]: active_chunks_) {
        auto [x, z] = chunkKeyToPos(key);
        glm::vec3 center(
            (x + 0.5f) * Constants::Chunks::CHUNK_SIZE,
            MAX_HEIGHT * 0.5f,
            (z + 0.5f) * Constants::Chunks::CHUNK_SIZE
        );

        glm::vec3 extents(
            Constants::Chunks::CHUNK_SIZE * 0.5f,
            MAX_HEIGHT * 0.5f,
            Constants::Chunks::CHUNK_SIZE * 0.5f
        );

        if (frustrum.isInside(center, extents)) {
            chunk->draw(shader_);
        }
    }
    glBindVertexArray(0);
}

