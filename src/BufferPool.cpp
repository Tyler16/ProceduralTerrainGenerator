#include "BufferPool.h"
#include "GlobalConstants.h"
#include "RenderingStructs.h"

BufferPool::BufferPool(size_t num_buffers) {
    createSharedTriangles();

    glGenBuffers(1, &shared_ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shared_ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 shared_triangles_.size() * sizeof(Triangle),
                 shared_triangles_.data(), GL_STATIC_DRAW);

    for (int i = 0; i < num_buffers; ++i) {
        BufferSet curr_buffer_set;
        glGenVertexArrays(1, &(curr_buffer_set.vao));
        glGenBuffers(1, &(curr_buffer_set.vbo));

        glBindVertexArray(curr_buffer_set.vao);

        glBindBuffer(GL_ARRAY_BUFFER, curr_buffer_set.vbo);
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
        buffer_stack_.push_back(curr_buffer_set);
    }
}

void BufferPool::createSharedTriangles() {
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

            Triangle t1 = { vert3, vert2, vert1 };
            shared_triangles_.push_back(t1);

            Triangle t2 = { vert2, vert3, vert4 };
            shared_triangles_.push_back(t2);
        }
    }
}

BufferSet BufferPool::acquire() {
    if (buffer_stack_.size() == 0) {
        return {0, 0};
    }

    BufferSet free_buffer = buffer_stack_[buffer_stack_.size() - 1];
    buffer_stack_.pop_back();
    return free_buffer;
}

bool BufferPool::hasBuffer() {
    return buffer_stack_.size() > 0;
}

void BufferPool::release(BufferSet buffer_set) {
    buffer_stack_.push_back(buffer_set);
}
