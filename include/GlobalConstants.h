#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

namespace Constants {
    namespace Chunks {
        inline constexpr unsigned int RESOLUTION = 64;
        inline constexpr unsigned int VERTS_PER_SIDE = RESOLUTION + 1;
        inline constexpr unsigned int NUM_VERTS = VERTS_PER_SIDE * VERTS_PER_SIDE;
        inline constexpr unsigned int PADDED_RESOLUTION = RESOLUTION + 2;
        inline constexpr float CHUNK_SIZE = 64.0f;
        inline constexpr unsigned int NUM_TRIANGLES = RESOLUTION * RESOLUTION * 2;
        inline constexpr float VERTEX_DIST = CHUNK_SIZE / (float) RESOLUTION;
    }

    namespace Window {
        inline constexpr int WIDTH = 800;
        inline constexpr int HEIGHT = 600;
    }
}

#endif
