#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Camera::init(glm::vec3 initial_position,
                  glm::vec3 front,
                  glm::vec3 up,
                  float yaw,
                  float pitch) {
    position_ = initial_position;
    front_ = front;
    up_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::vec3 Camera::getPosition() {
    return position_;
}

glm::vec3 Camera::getVelocity() {
    return velocity_;
}

void Camera::processMouse(float x_offset, float y_offset) {
    yaw_ += x_offset * SENSITIVITY;
    pitch_ += y_offset * SENSITIVITY;

    if (pitch_ > MAX_PITCH) pitch_ = MAX_PITCH;
    if (pitch_ < MIN_PITCH) pitch_ = MIN_PITCH;

    glm::vec3 unnormalized_front;
    unnormalized_front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    unnormalized_front.y = sin(glm::radians(pitch_));
    unnormalized_front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

    front_ = glm::normalize(unnormalized_front);
}

void Camera::processKeyboard(int direction, float delta_time) {
    velocity_ = front_;

    if (direction == GLFW_KEY_A || direction == GLFW_KEY_D) {
        velocity_ = getRight();
    }

    if (direction == GLFW_KEY_A || direction == GLFW_KEY_S) {
        velocity_ *= -1.0f;
    }

    velocity_ *= BASE_SPEED;
    position_ += velocity_ * delta_time;
}


