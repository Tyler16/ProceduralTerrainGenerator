#include "Chunk.h"
#include "GlobalConstants.h"
#include "HeightGenerator.h"
#include "RenderingStructs.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <utility>
#include <vector>

Chunk::Chunk(HeightGenerator& height_generator, BufferSet buffers) :
    height_generator_(height_generator),
    buffer_set_(buffers),
    status_(ChunkStatus::IDLE) {
    vertices_.resize(Constants::Chunks::NUM_VERTS);

    for (int z = 0; z < Constants::Chunks::VERTS_PER_SIDE; ++z) {
        for (int x = 0; x < Constants::Chunks::VERTS_PER_SIDE; ++x) {
            int idx = meshIdx(x, z);
            
            vertices_[idx].position = glm::vec3(localToGlobal(x),
                                                0.0f,
                                                localToGlobal(z));
            
            // TexCoords should stay 0.0 -> 1.0
            vertices_[idx].tex_coords = {
                (float)x / Constants::Chunks::RESOLUTION,
                (float)z / Constants::Chunks::RESOLUTION
            };
        }
    }
}

ChunkStatus Chunk::getStatus() {
    return status_;
}

void Chunk::setStatus(ChunkStatus status) {
    status_ = status;
}

BufferSet Chunk::getBufferSet() {
    return buffer_set_;
}

std::pair<int, int> Chunk::getPos() {
    return {x_offset_, z_offset_};
}

float Chunk::localToGlobal(int local) {
    return Constants::Chunks::CHUNK_SIZE *
                ((float)local / Constants::Chunks::RESOLUTION);
}

int Chunk::heightMapIdx(int local_x, int local_z) {
    return local_z * Constants::Chunks::HEIGHT_MAP_SIDE_VERTS + local_x;
}

int Chunk::meshIdx(int local_x, int local_z) {
    return local_z * Constants::Chunks::VERTS_PER_SIDE + local_x;
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
                                 std::vector<float>& height_map) {
    float l_height = height_map[heightMapIdx(local_x - 1, local_z)];
    float r_height = height_map[heightMapIdx(local_x + 1, local_z)];
    float u_height = height_map[heightMapIdx(local_x, local_z + 1)];
    float d_height = height_map[heightMapIdx(local_x, local_z - 1)];

    return glm::normalize(glm::vec3(l_height - r_height,
                                    Constants::Chunks::VERTEX_DIST * 2.0f,
                                    d_height - u_height));
}

void Chunk::generateVertices() {
    if (status_ == ChunkStatus::RETIRED) {
        return;
    }
    status_ = ChunkStatus::GENERATING;

    std::vector<float> height_map = generateHeightMap();
    
    for (int z = 0; z <= Constants::Chunks::VERTS_PER_SIDE; ++z) {
        for (int x = 0; x < Constants::Chunks::VERTS_PER_SIDE; ++x) {
            int mesh_idx = meshIdx(x, z);
            int hmap_idx = heightMapIdx(x + 1, z + 1);

            vertices_[mesh_idx].position.y = height_map[hmap_idx];
            vertices_[mesh_idx].normal = calculateNormal(x + 1, z + 1, height_map);
        }
    }
}

void Chunk::setBufferData() {
    glBindBuffer(GL_ARRAY_BUFFER, buffer_set_.vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    vertices_.size() * sizeof(Vertex),
                    vertices_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    status_ = ChunkStatus::ACTIVE;
}

void Chunk::draw(ShaderProgram& shader) {
    if (status_ != ChunkStatus::ACTIVE) {
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 world_pos(x_offset_ * Constants::Chunks::CHUNK_SIZE,
                        0.0f,
                        z_offset_ * Constants::Chunks::CHUNK_SIZE);
    model = glm::translate(model, world_pos);
    shader.setMat4("model", model);

    glBindVertexArray(buffer_set_.vao);
    glDrawElements(GL_TRIANGLES, Constants::Chunks::NUM_TRIANGLES * 3,
                   GL_UNSIGNED_INT, 0);
}

void Chunk::setup(int chunk_x, int chunk_z) {
    x_offset_ = chunk_x;
    z_offset_ = chunk_z;
    generateVertices();
}
