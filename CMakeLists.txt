cmake_minimum_required(VERSION 3.10)
project(ProceduralTerrainGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

include_directories(include)

add_executable(ProceduralTerrainGenerator
    src/main.cpp
    src/glad.c
)

target_link_libraries(ProceduralTerrainGenerator glfw)
target_link_libraries(ProceduralTerrainGenerator glm::glm)

if(APPLE)
    target_link_libraries(ProceduralTerrainGenerator "-framework OpenGL")
    target_link_libraries(ProceduralTerrainGenerator "-framework Cocoa")
    target_link_libraries(ProceduralTerrainGenerator "-framework IOKit")
    target_link_libraries(ProceduralTerrainGenerator "-framework CoreVideo")
endif()
