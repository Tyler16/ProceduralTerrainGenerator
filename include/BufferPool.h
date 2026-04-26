#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include "RenderingStructs.h"

#include <glad/glad.h>
#include <vector>

class BufferPool {
  public:
    BufferPool(size_t num_buffers);
    BufferSet acquire();
    bool hasBuffer();
    void release(BufferSet buffer_set);

  private:
    std::vector<BufferSet> buffer_stack_;
    GLuint shared_ebo_;
    static std::vector<Triangle> shared_triangles_;

    void createSharedTriangles();

};

#endif
