#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>

struct Plane {
    glm::vec3 normal;
    float distance;

    void normalize();

    float getDistance(const glm::vec3& point);
};

#endif
