#include "Frustrum.h"

#include "Plane.h"

#include <glm/glm.hpp>

Frustrum::Frustrum(const glm::mat4& m) {
    planes_[0].normal.x = m[0][3] + m[0][0];
    planes_[0].normal.y = m[1][3] + m[1][0];
    planes_[0].normal.z = m[2][3] + m[2][0];
    planes_[0].distance = m[3][3] + m[3][0];

    planes_[1].normal.x = m[0][3] - m[0][0];
    planes_[1].normal.y = m[1][3] - m[1][0];
    planes_[1].normal.z = m[2][3] - m[2][0];
    planes_[1].distance = m[3][3] - m[3][0];

    planes_[2].normal.x = m[0][3] + m[0][1];
    planes_[2].normal.y = m[1][3] + m[1][1];
    planes_[2].normal.z = m[2][3] + m[2][1];
    planes_[2].distance = m[3][3] + m[3][1];

    planes_[3].normal.x = m[0][3] - m[0][1];
    planes_[3].normal.y = m[1][3] - m[1][1];
    planes_[3].normal.z = m[2][3] - m[2][1];
    planes_[3].distance = m[3][3] - m[3][1];

    planes_[4].normal.x = m[0][3] + m[0][2];
    planes_[4].normal.y = m[1][3] + m[1][2];
    planes_[4].normal.z = m[2][3] + m[2][2];
    planes_[4].distance = m[3][3] + m[3][2];

    planes_[5].normal.x = m[0][3] - m[0][2];
    planes_[5].normal.y = m[1][3] - m[1][2];
    planes_[5].normal.z = m[2][3] - m[2][2];
    planes_[5].distance = m[3][3] - m[3][2];

    for (int i = 0; i < NUM_PLANES; ++i) {
        planes_[i].normalize();
    }
}

bool Frustrum::isInside(const glm::vec3& center, const glm::vec3& extents) {
    for (int i = 0; i < NUM_PLANES; i++) {
        Plane& plane = planes_[i];

        float r = extents.x * std::abs(plane.normal.x) +
                  extents.y * std::abs(plane.normal.y) +
                  extents.z * std::abs(plane.normal.z);

        // Distance from center to plane
        float d = plane.getDistance(center);

        // If the box is entirely on the negative side of any one plane, it's out
        if (d < -r) return false;
    }
    return true;
}

