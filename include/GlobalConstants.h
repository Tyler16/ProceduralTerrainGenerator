#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Constants {
    namespace Chunks {
        inline constexpr unsigned int RESOLUTION = 64;
        inline constexpr unsigned int VERTS_PER_SIDE = RESOLUTION + 1;
        inline constexpr unsigned int NUM_VERTS = VERTS_PER_SIDE * VERTS_PER_SIDE;
        inline constexpr unsigned int PADDED_RESOLUTION = RESOLUTION + 2;
        inline constexpr unsigned int HEIGHT_MAP_SIDE_VERTS = PADDED_RESOLUTION + 1;
        inline constexpr unsigned int HEIGHT_MAP_VERTS = HEIGHT_MAP_SIDE_VERTS * HEIGHT_MAP_SIDE_VERTS;
        inline constexpr float CHUNK_SIZE = 64.0f;
        inline constexpr unsigned int NUM_TRIANGLES = RESOLUTION * RESOLUTION * 2;
        inline constexpr float VERTEX_DIST = CHUNK_SIZE / (float) RESOLUTION;
    }

    namespace Window {
        inline constexpr int WIDTH = 1440;
        inline constexpr int HEIGHT = 900;
    }

    namespace Matrices {
        const glm::mat4 PROJECTION = glm::perspective(glm::radians(45.0f), (float) Window::WIDTH / (float) Window::HEIGHT, 0.1f, 1000.0f);;
        const glm::vec3 LIGHT_POS(1.2f, 0.5f, 2.0f);
    }

    namespace Colors {
        const glm::vec3 FOG(0.7f, 0.9f, 1.0f);
    }
}

#endif
