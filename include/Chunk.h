#ifndef CHUNK_H
#define CHUNK_H

#include "HeightGenerator.h"
#include "RenderingStructs.h"

#include <glm/glm.hpp>
#include <vector>

class Chunk {
  public:
    Chunk(int chunk_x,
          int chunk_z,
          BufferSet buffer_set,
          HeightGenerator& height_generator);

    void draw();
    void generateVertices();
    void resetBuffer();

  private:
    BufferSet buffer_set_;
    int x_offset_;
    int z_offset_;
    int seed_;
    std::atomic<bool> is_generated_;
    std::vector<Vertex> vertices_;
    HeightGenerator& height_generator_;

    float getHeight(float global_x, float global_z);
    float localToGlobal(int local, int offset);
    int localToIdx(int local_x, int local_z);
    std::vector<float> generateHeightMap();
    glm::vec3 calculateNormal(int local_x, int local_z,
                              std::vector<float> height_map);
};

#endif
