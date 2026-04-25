#include "Chunk.h"
#include "FastNoiseLite.h"
#include "GlobalConstants.h"
#include "HeightGenerator.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

Chunk::Chunk(int chunk_x,
             int chunk_z,
             BufferSet* buffer_set,
             HeightGenerator& noise) :
    x_offset_(chunk_x), z_offset_(chunk_z),
    buffer_set_(buffer_set), noise_(noise) {
    is_generated_ = false;
}

float Chunk::getHeight(float global_x, float global_z) {
    return noise_.getHeight(global_x, global_z);
}

float Chunk::localToGlobal(int local, int offset) {
    return Constants::Chunks::CHUNK_SIZE *
                (((float)local / Constants::Chunks::RESOLUTION) + offset);
}

int Chunk::localToIdx(int local_x, int local_z) {
    return local_z * Constants::Chunks::PADDED_RESOLUTION + local_x;
}

std::vector<float> Chunk::generateHeightMap() {
    std::vector<float> height_map;

    for (int local_z = 0; local_z <= Constants::Chunks::PADDED_RESOLUTION; ++local_z) {
        for (int local_x = 0; local_x <= Constants::Chunks::PADDED_RESOLUTION; ++local_x) {
            float global_x = localToGlobal(local_x - 1, x_offset_);
            float global_z = localToGlobal(local_z - 1, z_offset_);
            float global_y = getHeight(global_x, global_y);
            height_map.push_back(global_y);
        }
    }

    return height_map;
}

glm::vec3 Chunk::calculateNormal(int local_x, int local_z,
                                 std::vector<float> height_map) {
    float l_height = height_map[localToIdx(local_x - 1, local_z)];
    float r_height = height_map[localToIdx(local_x + 1, local_z)];
    float u_height = height_map[localToIdx(local_x, local_z + 1)];
    float d_height = height_map[localToIdx(local_x, local_z - 1)];

    return glm::normalize(glm::vec3(l_height - r_height,
                                    Constants::Chunks::VERTEX_DIST * 2,
                                    d_height - u_height));
}

void Chunk::generateVertices() {
    std::vector<float> height_map = generateHeightMap();
    
    for (int local_z = 1; local_z <= Constants::Chunks::PADDED_RESOLUTION - 1; ++local_z) {
        for (int local_x = 1; local_x <= Constants::Chunks::PADDED_RESOLUTION - 1; ++local_x) {
            Vertex v;

            float height = height_map[localToIdx(local_x, local_z)];
            v.position = glm::vec3(localToGlobal(local_x - 1, x_offset_),
                                   height,
                                   localToGlobal(local_z - 1, z_offset_));
            v.normal = calculateNormal(local_x, local_z, height_map);
            v.tex_coords = {
                (float) (local_x - 1) / Constants::Chunks::RESOLUTION,
                (float) (local_z - 1) / Constants::Chunks::RESOLUTION
            };
            vertices_.push_back(v);
        }
    }
    is_generated_ = true;
}

void Chunk::draw() {
    if (!is_generated_) {
        return;
    }
    glBindVertexArray(buffer_set_->vao);
    glDrawElements(GL_TRIANGLES, Constants::Chunks::NUM_TRIANGLES * 3,
                   GL_UNSIGNED_INT, 0);
}
