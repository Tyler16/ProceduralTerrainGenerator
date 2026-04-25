#ifndef RENDERING_DATA_H
#define RENDERING_DATA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

struct BufferSet {
    GLuint vao;
    GLuint vbo;
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
