#include "Chunk.h"
#include "GlobalConstants.h"
#include "HeightGenerator.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vector>

Chunk::Chunk(int chunk_x,
             int chunk_z,
             HeightGenerator& height_generator) :
    x_offset_(chunk_x), z_offset_(chunk_z),
    height_generator_(height_generator), ready_(false) {
}

bool Chunk::isReady() {
    return ready_;
}

BufferSet Chunk::getBufferSet() {
    return buffer_set_;
}

std::pair<int, int> Chunk::getPos() {
    return {x_offset_, z_offset_};
}

float Chunk::getHeight(float global_x, float global_z) {
    return global_x;
}

float Chunk::localToGlobal(int local, int offset) {
    return Constants::Chunks::CHUNK_SIZE *
                (((float)local / Constants::Chunks::RESOLUTION) + offset);
}

int Chunk::localToIdx(int local_x, int local_z) {
    return local_z * (Constants::Chunks::PADDED_RESOLUTION + 1) + local_x;
}

std::vector<float> Chunk::generateHeightMap() {
    std::vector<float> height_map(Constants::Chunks::HEIGHT_MAP_VERTS);
    

    height_generator_.getHeightMap(height_map.data(),
                                   x_offset_,
                                   z_offset_,
                                   Constants::Chunks::HEIGHT_MAP_SIDE_VERTS);

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
}

void Chunk::setBufferData(BufferSet buffer_set) {
    
    buffer_set_ = buffer_set;
    glBindBuffer(GL_ARRAY_BUFFER, buffer_set_.vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    vertices_.size() * sizeof(Vertex),
                    vertices_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vertices_.clear();
    ready_ = true;
}

void Chunk::draw() {
    if (!ready_) {
        return;
    }
    glBindVertexArray(buffer_set_.vao);
    glDrawElements(GL_TRIANGLES, Constants::Chunks::NUM_TRIANGLES * 3,
                   GL_UNSIGNED_INT, 0);
}
