#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
  private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    float yaw_;
    float pitch_;
    glm::vec3 velocity_;

    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float MAX_PITCH = 89.0f;
    static constexpr float MIN_PITCH = -89.0f;
    static constexpr float BASE_SPEED = 40.0f;

    glm::vec3 getRight() {
        return glm::normalize(glm::cross(front_, up_));
    }

    Camera() = default;

  public:
    Camera(const Camera&) = delete;
    void operator=(const Camera&) = delete;

    static Camera& getInstance() {
        static Camera instance;
        return instance;
    }

    void init(glm::vec3 position,
              glm::vec3 front,
              glm::vec3 up,
              float yaw,
              float pitch);

    glm::mat4 getViewMatrix();

    glm::vec3 getPosition();

    glm::vec3 getVelocity();

    void processMouse(float x_offset, float y_offset);

    void processKeyboard(int direction, float delta_time);

};

#endif
