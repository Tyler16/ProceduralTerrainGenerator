cmake_minimum_required(VERSION 3.10)
project(ProceduralTerrainGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

include_directories(include)

add_subdirectory(external/FastNoise2)

add_executable(ProceduralTerrainGenerator
    src/main.cpp
    src/Shader.cpp
    src/ShaderProgram.cpp
    src/Camera.cpp
    src/Chunk.cpp
    src/ChunkManager.cpp
    src/HeightGenerator.cpp
    src/ThreadPool.cpp
    src/BufferPool.cpp
    src/glad.c
)

target_link_libraries(ProceduralTerrainGenerator PRIVATE 
    glfw
    glm::glm
    FastNoise
)

target_include_directories(ProceduralTerrainGenerator PRIVATE external/FastNoise2/include)

if(APPLE)
    target_link_libraries(ProceduralTerrainGenerator PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

set(SHADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl"
)
set(CMAKE_BUILD_TYPE Debug)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
