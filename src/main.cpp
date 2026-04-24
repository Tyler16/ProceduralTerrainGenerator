#include "Camera.h"
#include "GlobalConstants.h"
#include "FastNoiseLite.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct Triangle {
    unsigned int vertices[3];
};

constexpr float CAMERA_INITIAL_YAW = -90.0f;
constexpr float CAMERA_INITIAL_PITCH = 0.0f;
Camera* camera = nullptr;

// Mouse ant time setup
float last_x = 400, last_y = 300;
bool first_mouse = true;
float delta_time = 0.0f, last_frame = 0.0f;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

// Callback for mouse movements
void mouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in) {
    float x_pos = static_cast<float>(x_pos_in);
    float y_pos = static_cast<float>(y_pos_in);

    if (first_mouse) {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos;
    last_x = x_pos;
    last_y = y_pos;

    camera->processMouse(x_offset, y_offset);
}

void processKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    std::array<int, 4> movement_inputs = {
        GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D
    };

    for (int& input : movement_inputs) {
        if (glfwGetKey(window, input) == GLFW_PRESS)
            camera->processKeyboard(input, delta_time);
    }
}

std::vector<Vertex> generateVertices(int resolution, float size, FastNoiseLite& noise, int x_off, int y_off) {
    std::vector<Vertex> vertices;

    for (int x = 0; x <= resolution; x++) {
        for (int z = 0; z <= resolution; z++) {
            float x_pos = size * (((float)x / resolution) + x_off);
            float z_pos = size * (((float)z / resolution) + y_off);
            float y_pos = (noise.GetNoise(x_pos, z_pos) + 1.0f) * 25.0f;
            //float y_pos = 0.0f;

            Vertex currVertex;
            currVertex.position = { x_pos, y_pos, z_pos };
            currVertex.normal = { 0.0f, 1.0f, 0.0f };
            currVertex.tex_coords = { (float) x / resolution, (float) z / resolution };
            
            currVertex.normal = {
                (noise.GetNoise((float) x_pos - 1.0f, (float) z_pos) - 
                noise.GetNoise((float) x_pos + 1.0f, (float) z_pos)) * 25.0f,
                2.0f,
                (noise.GetNoise((float) x_pos, (float) z_pos - 1.0f) - 
                noise.GetNoise((float) x_pos, (float) z_pos + 1.0f)) * 25.0f
            };
            currVertex.normal = glm::normalize(currVertex.normal);
            vertices.push_back(currVertex);
        }
    }

    return vertices;
}

std::vector<Triangle> generateTriangles(int resolution) {
    std::vector<Triangle> triangles;

    for (unsigned int x = 0; x < resolution; x++) {
        for (unsigned int z = 0; z < resolution; z++) {
            unsigned int row1 = x * (resolution + 1);
            unsigned int row2 = row1 + (resolution + 1);
            unsigned int vert1 = row1 + z;
            unsigned int vert2 = row2 + z;
            unsigned int vert3 = vert1 + 1;
            unsigned int vert4 = vert2 + 1;

            Triangle t1 = { vert3, vert2, vert1 };
            triangles.push_back(t1);

            Triangle t2 = { vert2, vert3, vert4 };
            triangles.push_back(t2);
        }
    }
    
    return triangles;
}

void calculateVertexNormals(std::vector<Vertex>& vertices, std::vector<Triangle>& triangles) {
    for (Triangle& triangle : triangles) {
        Vertex& v1 = vertices[triangle.vertices[0]];
        Vertex& v2 = vertices[triangle.vertices[1]];
        Vertex& v3 = vertices[triangle.vertices[2]];

        glm::vec3 edge1 = v2.position - v1.position;
        glm::vec3 edge2 = v3.position - v1.position;

        glm::vec3 faceNormal = glm::cross(edge1, edge2);

        v1.normal += faceNormal;
        v2.normal += faceNormal;
        v3.normal += faceNormal;
    }

    for (Vertex& v : vertices) {
        v.normal = glm::normalize(v.normal);
    }
}

int main() {
    // Mac initialization boilerplate
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Initialize window
    GLFWwindow* window = glfwCreateWindow(
        Constants::Window::WIDTH, Constants::Window::HEIGHT,
        "Infinite Terrain", NULL, NULL
    );
    glfwMakeContextCurrent(window);
    
    // Initialize glad and mouse settings
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Initialize camera
    const glm::vec3 CAMERA_INITIAL_POS = glm::vec3(0.0f, 100.0f, 0.0f);
    const glm::vec3 CAMERA_INITIAL_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 CAMERA_INITIAL_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    camera = new Camera(CAMERA_INITIAL_POS,
                       CAMERA_INITIAL_FRONT,
                       CAMERA_INITIAL_UP,
                       CAMERA_INITIAL_YAW,
                       CAMERA_INITIAL_PITCH);

    // Initialize FastNoiseLite
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(5);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    // Create and compile shaders
    Shader vertexShader = Shader(GL_VERTEX_SHADER, "shaders/vertex.glsl");
    Shader fragmentShader = Shader(GL_FRAGMENT_SHADER, "shaders/fragment.glsl");
    ShaderProgram shader_program = ShaderProgram();
    shader_program.addShader(vertexShader);
    shader_program.addShader(fragmentShader);
    shader_program.build();

    // Generate vertices and triangles
    int resolution = 32;
    float size = 32.0f;
    std::vector<Vertex> vertices = generateVertices(resolution, size, noise, 0, 0);
    std::vector<Triangle> triangles = generateTriangles(resolution);
    std::vector<Vertex> vertices2 = generateVertices(resolution, size, noise, 0, 1);
    //std::vector<Triangle> triangles2 = generateTriangles(resolution);
    //calculateVertexNormals(vertices, triangles);
    //calculateVertexNormals(vertices2, triangles2);
 
    // Setup buffers
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO 
    glBindVertexArray(VAO);

    // Set up VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Set up arrays in VBO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    // Set up EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), triangles.data(), GL_STATIC_DRAW);

    // Unbind VAO
    glBindVertexArray(0);

        // Setup buffers
    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    // Bind VAO 
    glBindVertexArray(VAO2);

    // Set up VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(Vertex), vertices2.data(), GL_STATIC_DRAW);

    // Set up arrays in VBO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    // Set up EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Unbind VAO
    glBindVertexArray(0);


    while (!glfwWindowShouldClose(window)) {
        // Calculate Time
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Process keyboard input
        processKeyboard(window);

        // Rendering Commands
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_program.use();

        // Set up camera matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        shader_program.setMat4("projection", projection);
        shader_program.setMat4("view", view);
        shader_program.setMat4("model", model);
        shader_program.setMat3("normalMatrix", normalMatrix);

        // Set up lighting
        glm::vec3 light_pos(1.2f, 50.0f, 2.0f);
        glm::vec3 light_color(1.0f, 1.0f, 0.9f);
        shader_program.setVec3("lightPos", light_pos);
        shader_program.setVec3("lightColor", light_color);
        shader_program.setVec3("viewPos", camera->getPosition());
        shader_program.setVec3("objectColor", glm::vec3(0.2f, 0.5f, 0.2f));

        // Draw the Terrain
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
