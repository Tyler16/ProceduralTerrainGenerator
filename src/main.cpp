#include "Camera.h"
#include "ChunkManager.h"
#include "GlobalConstants.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

constexpr float CAMERA_INITIAL_YAW = -90.0f;
constexpr float CAMERA_INITIAL_PITCH = 0.0f;
Camera* camera = nullptr;

float last_x = 400, last_y = 300;
bool first_mouse = true;
float delta_time = 0.0f, last_frame = 0.0f;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        Constants::Window::WIDTH, Constants::Window::HEIGHT,
        "Infinite Terrain", NULL, NULL
    );
    glfwMakeContextCurrent(window);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const glm::vec3 CAMERA_INITIAL_POS = glm::vec3(0.0f, 100.0f, 0.0f);
    const glm::vec3 CAMERA_INITIAL_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 CAMERA_INITIAL_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    camera = new Camera(CAMERA_INITIAL_POS,
                       CAMERA_INITIAL_FRONT,
                       CAMERA_INITIAL_UP,
                       CAMERA_INITIAL_YAW,
                       CAMERA_INITIAL_PITCH);

    Shader vertexShader = Shader(GL_VERTEX_SHADER, "shaders/vertex.glsl");
    Shader fragmentShader = Shader(GL_FRAGMENT_SHADER, "shaders/fragment.glsl");
    ShaderProgram shader_program = ShaderProgram();
    shader_program.addShader(vertexShader);
    shader_program.addShader(fragmentShader);
    shader_program.build();

    ChunkManager chunk_manager = ChunkManager(67);

    while (!glfwWindowShouldClose(window)) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        processKeyboard(window);
        chunk_manager.update(camera->getPosition(), current_frame);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_program.use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        shader_program.setMat4("projection", projection);
        shader_program.setMat4("view", view);
        shader_program.setMat4("model", model);
        shader_program.setMat3("normalMatrix", normalMatrix);

        glm::vec3 light_pos(1.2f, 50.0f, 2.0f);
        glm::vec3 light_color(1.0f, 1.0f, 0.9f);
        shader_program.setVec3("lightPos", light_pos);
        shader_program.setVec3("lightColor", light_color);
        shader_program.setVec3("viewPos", camera->getPosition());
        shader_program.setVec3("objectColor", glm::vec3(0.2f, 0.5f, 0.2f));

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        chunk_manager.render();
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
