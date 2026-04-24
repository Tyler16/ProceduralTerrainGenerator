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

    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float MAX_PITCH = 89.0f;
    static constexpr float MIN_PITCH = -89.0f;
    static constexpr float BASE_SPEED = 20.0f;

    glm::vec3 getRight() {
        return glm::normalize(glm::cross(front_, up_));
    }

  public:
    Camera(glm::vec3 position,
           glm::vec3 front,
           glm::vec3 up,
           float yaw,
           float pitch)
        : position_(position), front_(front), up_(up), yaw_(yaw), pitch_(pitch) {}

    glm::mat4 getViewMatrix();

    glm::vec3 getPosition();

    void processMouse(float x_offset, float y_offset);

    void processKeyboard(int direction, float delta_time);

};

#endif
