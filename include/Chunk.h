#ifndef CHUNK_H
#define CHUNK_H

#include "HeightGenerator.h"
#include "RenderingStructs.h"

#include <glm/glm.hpp>

#include <vector>
#include <utility>

enum class ChunkStatus {
    IDLE,
    PENDING,
    GENERATING,
    ACTIVE,
    RETIRED
};

class Chunk {
  public:
    Chunk(HeightGenerator& height_generator, BufferSet buffers);

    void draw();
    void setup(int chunk_x, int chunk_z);
    void setBufferData();
    BufferSet getBufferSet();
    std::pair<int, int> getPos();
    ChunkStatus getStatus();
    void setStatus(ChunkStatus status);

  private:
    BufferSet buffer_set_;
    HeightGenerator& height_generator_;
    std::vector<Vertex> vertices_;
    std::atomic<ChunkStatus> status_;
    int x_offset_;
    int z_offset_;
    int seed_;

    float getHeight(float global_x, float global_z);
    float localToGlobal(int local, int offset);
    int localToIdx(int local_x, int local_z);
    std::vector<float> generateHeightMap();
    glm::vec3 calculateNormal(int local_x, int local_z,
                              std::vector<float> height_map);
    void generateVertices();
};

#endif
