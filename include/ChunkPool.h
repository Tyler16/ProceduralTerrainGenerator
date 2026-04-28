#ifndef CHUNK_POOL_H
#define CHUNK_POOL_H

#include "Chunk.h"
#include "HeightGenerator.h"
#include "RenderingStructs.h"

#include <glad/glad.h>

#include <cstddef>
#include <memory>
#include <vector>

class ChunkPool {

  public:
    ChunkPool(std::size_t num_chunks, HeightGenerator& height_generator);
    bool hasChunk();
    std::shared_ptr<Chunk> getChunk();
    void returnChunk(std::shared_ptr<Chunk> ptr);

  private:
    std::vector<Triangle> shared_triangles_;
    std::vector<std::shared_ptr<Chunk>> chunk_stack_;
    GLuint shared_ebo_;

    void createSharedTriangles();
    void createChunks(std::size_t num_chunks, HeightGenerator& height_generator);
    BufferSet createBufferSet();
};

#endif
