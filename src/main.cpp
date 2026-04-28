#include "Camera.h"
#include "ChunkManager.h"
#include "GlobalConstants.h"
#include "ShaderProgram.h"
#include "Skybox.h"

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


    ShaderProgram chunk_shader = ShaderProgram("shaders/chunk.vert",
                                               "shaders/chunk.frag");
 
    ShaderProgram skybox_shader = ShaderProgram("shaders/skybox.vert",
                                                "shaders/skybox.frag");

    Skybox skybox = Skybox(skybox_shader);
    ChunkManager chunk_manager = ChunkManager(67, chunk_shader);

    while (!glfwWindowShouldClose(window)) {
        float current_frame = (float) glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        processKeyboard(window);
        chunk_manager.update(camera->getPosition(), current_frame);

        glClearColor(Constants::Colors::FOG[0],
                     Constants::Colors::FOG[1],
                     Constants::Colors::FOG[2],
                     1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 sky_view = glm::mat4(glm::mat3(view)); 
        skybox.render(sky_view, current_frame);
        chunk_manager.render(view, camera->getPosition(), current_frame);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
