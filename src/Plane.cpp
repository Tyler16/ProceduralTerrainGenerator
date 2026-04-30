#include "Plane.h"

void Plane::normalize() {
    float length = glm::length(normal);
    normal /= length;
    distance /= length;
}

float Plane::getDistance(const glm::vec3& point) {
    return glm::dot(normal, point) + distance;
}
