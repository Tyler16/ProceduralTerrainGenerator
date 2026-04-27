#include "ChunkPool.h"

#include "GlobalConstants.h"
#include "HeightGenerator.h"
#include "RenderingStructs.h"

#include <cstddef>
#include <memory>

ChunkPool::ChunkPool(std::size_t num_chunks, HeightGenerator& height_generator) {
    createSharedTriangles();
    createChunks(num_chunks, height_generator);
}

void ChunkPool::createSharedTriangles() {
    if (shared_triangles_.size() > 0) {
        return;
    }

    for (unsigned int x = 0; x < Constants::Chunks::RESOLUTION; ++x) {
        for (unsigned int z = 0; z < Constants::Chunks::RESOLUTION; ++z) {
            unsigned int row1 = x * (Constants::Chunks::RESOLUTION + 1);
            unsigned int row2 = row1 + (Constants::Chunks::RESOLUTION + 1);
            unsigned int vert1 = row1 + z;
            unsigned int vert2 = row2 + z;
            unsigned int vert3 = vert1 + 1;
            unsigned int vert4 = vert2 + 1;

            Triangle t1 = { vert1, vert2, vert3 };
            shared_triangles_.push_back(t1);

            Triangle t2 = { vert4, vert3, vert2 };
            shared_triangles_.push_back(t2);
        }
    }

    glGenBuffers(1, &shared_ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shared_ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 shared_triangles_.size() * sizeof(Triangle),
                 shared_triangles_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

BufferSet ChunkPool::createBufferSet() {
    BufferSet buffer_set;
    glGenVertexArrays(1, &(buffer_set.vao));
    glGenBuffers(1, &(buffer_set.vbo));

    glBindVertexArray(buffer_set.vao);

    glBindBuffer(GL_ARRAY_BUFFER, buffer_set.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 Constants::Chunks::NUM_VERTS * sizeof(Vertex),
                 nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, tex_coords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shared_ebo_);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return buffer_set;
}

void ChunkPool::createChunks(std::size_t num_chunks,
                             HeightGenerator& height_generator) {
    for (int i = 0; i < num_chunks; ++i) {
        BufferSet curr_buffers = createBufferSet();

        std::shared_ptr<Chunk> ptr = std::make_shared<Chunk>(height_generator,
                                                             curr_buffers);
        chunk_stack_.push_back(std::move(ptr));
    }
}

bool ChunkPool::hasChunk() {
    return !chunk_stack_.empty();
}

std::shared_ptr<Chunk> ChunkPool::getChunk() {
    if (chunk_stack_.empty()) {
        return nullptr;
    }

    std::shared_ptr<Chunk> res = std::move(chunk_stack_.back());
    res->setStatus(ChunkStatus::PENDING);
    chunk_stack_.pop_back();
    return res;
}

void ChunkPool::returnChunk(std::shared_ptr<Chunk> ptr) {
    ptr->setStatus(ChunkStatus::IDLE);
    chunk_stack_.push_back(std::move(ptr));
}
