#ifndef RENDERING_DATA_H
#define RENDERING_DATA_H

#include <glm/glm.hpp>

struct BufferSet {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    bool in_use;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct Triangle {
    unsigned int vertices[3];
};

#endif
